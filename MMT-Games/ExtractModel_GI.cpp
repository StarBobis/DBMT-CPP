#include "Functions_GI.h"

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

void Functions_GI::ExtractModel(){
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    for (const auto& DrawIB_Pair : this->DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = DrawIB_Pair.first;
        DrawIBConfig extractConfig = DrawIB_Pair.second;
        this->InitializeGameType(extractConfig);

        extractConfig.CreateDrawIBOutputFolder(G.Path_OutputFolder);

        D3D11GameType d3d11GameType = d3d11GameTypeLv2.GameTypeName_D3d11GameType_Map[MMTString::ToByteString(extractConfig.GameType)];


        VertexCountType HI3VertexCountType = VertexCountType::BufFileRemoveNull;
        std::map<std::wstring, uint64_t> TrianglelistIndexVertexCountMap = FAData.Get_TrianglelistIndex_VertexCount_Map(HI3VertexCountType, extractConfig.DrawIB);
        uint64_t TranglelistMaxVertexCount = FAData.GetTrianglelistMaxVertexCount(HI3VertexCountType, extractConfig.DrawIB);
        std::wstring TrianglelistExtractIndex = FAData.GetTrianglelistExtractIndex(HI3VertexCountType, extractConfig.DrawIB, d3d11GameType, true);
        std::wstring PointlistExtractIndex = FAData.GetPointlistExtractIndex(HI3VertexCountType, TranglelistMaxVertexCount);

        std::string VertexLimitVB = MMTString::ToByteString(FAData.FindFrameAnalysisFileNameListWithCondition(TrianglelistExtractIndex + L"-vb0", L".txt")[0].substr(11, 8));
        extractConfig.VertexLimitVB = VertexLimitVB;
        LOG.NewLine();

        //����Categoryֱ�ӴӶ�Ӧ������ȡ��ӦBuffer�ļ�
        LOG.Info("Extract from Buffer file: ");
        std::vector<std::unordered_map<uint64_t, std::vector<std::byte>>> categoryFileBufList;

        std::unordered_map<std::string, std::string> CategoryHashMap;
        for (std::string Category : d3d11GameType.OrderedCategoryNameList) {
            std::string CategorySlot = d3d11GameType.CategorySlotMap[Category];
            std::string CategoryTopology = d3d11GameType.CategoryTopologyMap[Category];
            int CategoryStride = d3d11GameType.CategoryStrideMap[Category];
            std::wstring ExtractIndex = PointlistExtractIndex;

            if (CategoryTopology == "trianglelist") {
                ExtractIndex = TrianglelistExtractIndex;
            }

            //��ȡ��������Ӧ��λ��RealStride
            std::vector<std::wstring> filenames = FAData.FindFrameAnalysisFileNameListWithCondition(ExtractIndex + L"-" + MMTString::ToWideString(CategorySlot), L".txt");
            std::wstring filename = filenames[0];
            CategoryHashMap[Category] = MMTString::ToByteString(filename.substr(11, 8));
            VertexBufferDetect vbDetect(G.WorkFolder + filename);

            //����ʹ��fileRealStrideʱ���޷���֤txt������ʵ�����ݣ����Ե�����ȡʧ��
            // TODO �ҵ����Ӽ��ݵİ취
            //int stride = vbDetect.fileRealStride;

            int stride = vbDetect.fileShowStride;

            //�ж��Ƿ���Ҫ����Ĭ�ϵ�Blendweights
            //TODO ����ʶ���realElementNameList�Ǵ���ģ��������ھ�����
            //TODO ��������ȫ����Ϊ����Buffer��ʶ��������ʱ�����������
            bool patchBlendWeights = d3d11GameType.PatchBLENDWEIGHTS;
            
            

            //��ȡ������
            std::wstring bufFileName = filename.substr(0, filename.length() - 4) + L".buf";
            uint64_t bufFileSize = MMTFile::GetFileSize(G.WorkFolder + bufFileName);
            uint64_t vertexNumber = bufFileSize / stride;

            std::unordered_map<uint64_t, std::vector<std::byte>> fileBuf = MMTFile::ReadBufMapFromFile(G.WorkFolder + bufFileName, vertexNumber);
            LOG.Info(L"Extract from: " + bufFileName + L" VertexNumber:" + std::to_wstring(vertexNumber));
            LOG.Info("Category:" + Category + " CategorySlot:" + CategorySlot + " CategoryTopology:" + CategoryTopology);

            
            
            if (Category == "Blend" && patchBlendWeights) {
                LOG.Info(L"�Զ���ȫBLENDWEIGHT 1,0,0,0  Ŀǰ��֧��R32G32B32A32_FLOAT���Ͳ�ȫ��ĿǰҲֻ��������һ������");
                std::unordered_map<uint64_t, std::vector<std::byte>> patchedFileBuf = MMTFormat::PatchBlendBuf_BLENDWEIGHT_1000(fileBuf);
                
                categoryFileBufList.push_back(patchedFileBuf);

            }
            else if (Category == "Texcoord" && d3d11GameType.TexcoordPatchNull != 0) {
                LOG.Info(L"GI4.8������ �����Texcoord����Ϊ36�Ĵ�����������ִ�С�");
                //�����Texcoord����Ϊ36�Ĵ�����
                std::unordered_map<uint64_t, std::vector<std::byte>> RemovePatchedTexcoordBuf;
                for (const auto& pair : fileBuf) {
                    uint64_t num = pair.first;
                    std::vector<std::byte> bufValue = pair.second;
                    std::vector<std::byte> SubstrBufValue = MMTFormat::GetRange_Byte(pair.second,0, bufValue.size() - d3d11GameType.TexcoordPatchNull);
                    RemovePatchedTexcoordBuf[num] = SubstrBufValue;
                }
                categoryFileBufList.push_back(RemovePatchedTexcoordBuf);
            }
            else {
                categoryFileBufList.push_back(fileBuf);
            }




        }
        extractConfig.CategoryHashMap = CategoryHashMap;

        LOG.NewLine();
        //Buffer�ļ���ϳ�һ��
        std::vector<std::byte> finalVB0Buf;
        for (int i = 0; i < TranglelistMaxVertexCount; i++) {
            for (std::unordered_map<uint64_t, std::vector<std::byte>>& tmpFileBuf : categoryFileBufList) {
                finalVB0Buf.insert(finalVB0Buf.end(), tmpFileBuf[i].begin(), tmpFileBuf[i].end());
            }
        }

        extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;

        //����FMT�ļ�
        FmtFile fmtFileData;
        fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
        fmtFileData.d3d11GameType = d3d11GameType;
        fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
        fmtFileData.Stride = d3d11GameType.getElementListStride(fmtFileData.ElementNameList);
        LOG.NewLine();


        //IB��Buffer�ļ������������
        LOG.Info(L"Start to read trianglelist IB file info");
        // ��ȷ�����ڲ�ͬ��trianglelist indices��ib�ļ�������first index��ȷ�ϡ�
        std::map<int, std::wstring> firstIndexFileNameMap;
        for (const auto& TrianglelistPair:TrianglelistIndexVertexCountMap ) {
            std::vector<std::wstring> trianglelistIBFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(TrianglelistPair.first + L"-ib", L".txt");
            std::wstring trianglelistIBFileName;
            if (trianglelistIBFileNameList.empty()) {
                continue;
            }
            trianglelistIBFileName = trianglelistIBFileNameList[0];
            //LOG.Info(L"trianglelistIBFileName: " + trianglelistIBFileName);
            IndexBufferTxtFile indexBufferFileData = IndexBufferTxtFile(G.WorkFolder + trianglelistIBFileName, false);
            std::wstring firstIndex = indexBufferFileData.FirstIndex;
            //LOG.Info(L"firstIndex: " + firstIndex);
            firstIndexFileNameMap[std::stoi(firstIndex)] = trianglelistIBFileName;
        }
        LOG.Info(L"Output and see the final first index and trianglelist file:");
        std::vector<std::string> firstIndexList;
        for (auto it = firstIndexFileNameMap.begin(); it != firstIndexFileNameMap.end(); ++it) {
            LOG.Info(L"firstIndex: " + std::to_wstring(it->first) + L", trianglelistIBFileName: " + it->second);
            firstIndexList.push_back(std::to_string(it->first));
        }

        LOG.NewLine();

        //���ھͿ�������ˣ�����ÿ��MatchFirstIndex����������ɡ�
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
            ImportModelList.push_back(MMTString::ToByteString(DrawIB) + "-" + std::to_string(outputCount));

            //
            MatchFirstIndex_PartName_Map[ibFileData.FirstIndex] = std::to_wstring(outputCount);

            //�ֱ����fmt,ib,vb
            std::wstring OutputIBBufFilePath = extractConfig.DrawIBOutputFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
            std::wstring OutputVBBufFilePath = extractConfig.DrawIBOutputFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
            std::wstring OutputFmtFilePath = extractConfig.DrawIBOutputFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

            //���FMT�ļ�
            fmtFileData.OutputFmtFile(OutputFmtFilePath);

            //���IB�ļ�
            IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
            ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
            ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);

            //���VB�ļ�
            VertexBufferBufFile vbBufFile;
            vbBufFile.FinalVB0Buf = finalVB0Buf;
            vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
            vbBufFile.SaveToFile(OutputVBBufFilePath);

            //PartName������
            outputCount++;
        }

        extractConfig.MatchFirstIndexList = MatchFirstIndexList;
        extractConfig.PartNameList = PartNameList;
        extractConfig.ImportModelList = ImportModelList;

        //AutoDetectTextureFiles(DrawIB, MatchFirstIndex_PartName_Map,d3d11GameType.GPUPreSkinning);
        AutoDetectTextureFiles_GI(DrawIB, MatchFirstIndex_PartName_Map, d3d11GameType.GPUPreSkinning,this->PartName_TextureSlotReplace_Map,this->TextureTypeList);

        for (const auto& pair : this->PartName_TextureSlotReplace_Map) {
            extractConfig.PartName_TextureSlotReplace_Map[pair.first] = pair.second.Save_SlotName_Equal_ResourceFileName_List;
        }
        //���tmp.json
        extractConfig.SaveTmpJsonConfigs(extractConfig.DrawIBOutputFolder);
        this->PartName_TextureSlotReplace_Map.clear();

        LOG.NewLine();
    }

    //�ƶ�ͨ����ͼ����
    FunctionsBasicUtils::MoveAllUsedTexturesToOutputFolder(this->DrawIB_ExtractConfig_Map);

    LOG.Info("Extract model success");
}