#include "Functions_HI3.h"

#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtFile.h"
#include "FrameAnalysisData.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"

#include "MMTConstants.h"

#include "FunctionsBasicUtils.h"

#include "TextureUtils.h"

void Functions_HI3::ExtractModel() {
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    for (const auto& DrawIB_Pair : this->DrawIB_ExtractConfig_Map) {
        DrawIBConfig drawIBConfig = DrawIB_Pair.second;
        this->InitializeGameType(drawIBConfig);

        drawIBConfig.CreateDrawIBOutputFolder(G.Path_OutputFolder);

        D3D11GameType d3d11GameType = d3d11GameTypeLv2.GameTypeName_D3d11GameType_Map[MMTString::ToByteString(drawIBConfig.GameType)];

        VertexCountType HI3VertexCountType = VertexCountType::BufFileRemoveNull;
        std::map<std::wstring, uint64_t> TrianglelistIndexVertexCountMap = FAData.Get_TrianglelistIndex_VertexCount_Map(HI3VertexCountType,drawIBConfig.DrawIB);
        uint64_t TranglelistMaxVertexCount = FAData.GetTrianglelistMaxVertexCount(HI3VertexCountType, drawIBConfig.DrawIB);
        std::wstring TrianglelistExtractIndex = FAData.GetTrianglelistExtractIndex(HI3VertexCountType, drawIBConfig.DrawIB,d3d11GameType,true);
        std::wstring PointlistExtractIndex = FAData.GetPointlistExtractIndex(HI3VertexCountType, TranglelistMaxVertexCount);
      
        drawIBConfig.VertexLimitVB = MMTString::ToByteString(FAData.FindFrameAnalysisFileNameListWithCondition(TrianglelistExtractIndex + L"-vb0", L".txt")[0].substr(11, 8));
        LOG.Info("VertexLimitVB: " + drawIBConfig.VertexLimitVB);
        LOG.NewLine();

        //根据Category直接从对应索引提取对应Buffer文件
        LOG.Info("Extract from Buffer file: ");
        std::vector<std::unordered_map<uint64_t, std::vector<std::byte>>> categoryFileBufList;

        std::unordered_map<std::string, std::string> CategoryHashMap;
        for (std::string Category : d3d11GameType.OrderedCategoryNameList) {
            std::string CategorySlot = d3d11GameType.CategorySlotMap[Category];
            std::string CategoryTopology = d3d11GameType.CategoryTopologyMap[Category];
            std::wstring ExtractIndex = PointlistExtractIndex;

            if (CategoryTopology == "trianglelist") {
                ExtractIndex = TrianglelistExtractIndex;
            }

            //获取此索引对应槽位的RealStride
            std::vector<std::wstring> filenames = FAData.FindFrameAnalysisFileNameListWithCondition(ExtractIndex + L"-" + MMTString::ToWideString(CategorySlot), L".txt");
            std::wstring filename = filenames[0];
            CategoryHashMap[Category] = MMTString::ToByteString(filename.substr(11, 8));
            VertexBufferDetect vbDetect(G.WorkFolder + filename);

            //这里使用fileRealStride时，无法保证txt里是真实的数据，所以导致提取失败
            // TODO 找到更加兼容的办法
            //int stride = vbDetect.fileRealStride;

            int stride = vbDetect.fileShowStride;

            //判断是否需要补充默认的Blendweights
            std::vector<std::string> addElementList = vbDetect.realElementNameList;
            bool patchBlendWeights = false;
            patchBlendWeights = d3d11GameType.PatchBLENDWEIGHTS;
            
            //获取顶点数
            std::wstring bufFileName = filename.substr(0, filename.length() - 4) + L".buf";
            uint64_t bufFileSize = MMTFile::GetFileSize(G.WorkFolder + bufFileName);
            uint64_t vertexNumber = bufFileSize / stride;

            
            std::unordered_map<uint64_t, std::vector<std::byte>> fileBuf = MMTFile::ReadBufMapFromFile(G.WorkFolder + bufFileName, vertexNumber);
            LOG.Info(L"Extract from: " + bufFileName + L" VertexNumber:" + std::to_wstring(vertexNumber));
            LOG.Info("Category:" + Category + " CategorySlot:" + CategorySlot + " CategoryTopology:" + CategoryTopology);

            //自动补全BLENDWEIGHT 1,0,0,0  目前仅支持R32G32B32A32_FLOAT类型补全，目前也只遇到过这一种类型
           
            if (Category == "Blend" && patchBlendWeights) {
                std::unordered_map<uint64_t, std::vector<std::byte>> patchedFileBuf = MMTFormat::PatchBlendBuf_BLENDWEIGHT_1000(fileBuf);
                categoryFileBufList.push_back(patchedFileBuf);
            }
            else {
                categoryFileBufList.push_back(fileBuf);
            }


        }
        drawIBConfig.CategoryHashMap = CategoryHashMap;
        LOG.NewLine();

        //Buffer文件组合成一个
        std::vector<std::byte> finalVB0Buf;
        for (uint64_t i = 0; i < TranglelistMaxVertexCount; i++) {
            for (std::unordered_map<uint64_t, std::vector<std::byte>>& tmpFileBuf : categoryFileBufList) {
                finalVB0Buf.insert(finalVB0Buf.end(), tmpFileBuf[i].begin(), tmpFileBuf[i].end());
            }
        }

        drawIBConfig.TmpElementList = d3d11GameType.OrderedFullElementList;



        //生成FMT文件
        FmtFile fmtFileData;
        fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
        fmtFileData.d3d11GameType = d3d11GameType;
        fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
        fmtFileData.Stride = d3d11GameType.getElementListStride(d3d11GameType.OrderedFullElementList);
        LOG.NewLine();


        //IB的Buffer文件根据索引拆分
        LOG.Info(L"Start to read trianglelist IB file info");
        // 读确定与众不同的trianglelist indices的ib文件，根据first index来确认。

        std::map<uint64_t, std::wstring> firstIndexFileNameMap = FAData.Get_MatchFirstIndex_IBFileName_Map(drawIBConfig.DrawIB);
        
        LOG.Info(L"Output and see the final first index and trianglelist file:");
        std::vector<std::string> firstIndexList;
        for (auto it = firstIndexFileNameMap.begin(); it != firstIndexFileNameMap.end(); ++it) {
            LOG.Info(L"firstIndex: " + std::to_wstring(it->first) + L", trianglelistIBFileName: " + it->second);
            firstIndexList.push_back(std::to_string(it->first));
        }

        LOG.NewLine();

        //现在就可以输出了，根据每个MatchFirstIndex依次输出即可。
        int outputCount = 1;
        std::vector<std::string> MatchFirstIndexList;
        std::vector<std::string> PartNameList;
        std::vector<std::string> ImportModelList;
        std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map;
        for (const auto& pair : firstIndexFileNameMap) {
            std::wstring IBReadFileName = pair.second;
            std::wstring IBReadBufferFileName = IBReadFileName.substr(0, IBReadFileName.length() - 4) + L".buf";
            std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;

            std::wstring IBReadFilePath = G.WorkFolder + IBReadFileName;
            IndexBufferTxtFile ibFileData(IBReadFilePath, false);

            MatchFirstIndexList.push_back(MMTString::ToByteString(ibFileData.FirstIndex));
            PartNameList.push_back(std::to_string(outputCount));
            ImportModelList.push_back(MMTString::ToByteString(drawIBConfig.DrawIB) + "-" + std::to_string(outputCount));

            LOG.Info(L"IB Buffer ReadFileName: " + IBReadBufferFileName);
            MatchFirstIndex_PartName_Map[ibFileData.FirstIndex] = std::to_wstring(outputCount);

            //分别输出fmt,ib,vb
            std::wstring OutputIBBufFilePath = drawIBConfig.DrawIBOutputFolder + drawIBConfig.DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
            std::wstring OutputVBBufFilePath = drawIBConfig.DrawIBOutputFolder + drawIBConfig.DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
            std::wstring OutputFmtFilePath = drawIBConfig.DrawIBOutputFolder + drawIBConfig.DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

            //输出FMT文件
            fmtFileData.OutputFmtFile(OutputFmtFilePath);

            //输出IB文件
            IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
            ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
            ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);

            //输出VB文件
            VertexBufferBufFile vbBufFile;
            vbBufFile.FinalVB0Buf = finalVB0Buf;
            vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
            vbBufFile.SaveToFile(OutputVBBufFilePath);

            //PartName数自增
            outputCount++;
        }

        drawIBConfig.MatchFirstIndexList = MatchFirstIndexList;
        drawIBConfig.PartNameList = PartNameList;
        drawIBConfig.ImportModelList = ImportModelList;

        //测试V2版本自动贴图识别算法。
        AutoDetectTextureFilesV2_HI3(drawIBConfig);

        //输出tmp.json
        drawIBConfig.SaveTmpJsonConfigsV2(drawIBConfig.DrawIBOutputFolder);
        LOG.NewLine();
    }

    //移动通用贴图数据
    FunctionsBasicUtils::MoveAllUsedTexturesToOutputFolder(this->DrawIB_ExtractConfig_Map);
    LOG.Info("Extract HI3 model success");
}