#include "BufferUtils.h"

#include "VertexBufferBufFile.h"
#include "FmtFile.h"
#include "MMTConstants.h"

namespace BufferUtilsLv2 {
    //PartName_DrawIndexed_Map 
    //Deprecated
    //扩展到每个PartName可以有多个部位的新架构之后，这个就移除掉。
    std::unordered_map<std::string, M_DrawIndexed> Get_PartName_DrawIndexed_Map(DrawIBConfig IBConfig) {
        std::unordered_map<std::string, M_DrawIndexed> PartName_DrawIndexed_Map;

        LOG.Info(L"读取DrawIndexed数值");

        std::wstring readFormatFileName = IBConfig.BufferReadFolder + L"1.fmt";
        std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(readFormatFileName, L"format");
        LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);
        uint64_t Offset = 0;
        //读取ib文件,转换格式后直接写出
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".fmt";
            FmtFile fmtFile(partNameFmtFileName);
            if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                LOG.Error(L"当前读取的fmt文件: " + partNameFmtFileName + L"中的ElementName与当前提取用的数据类型的GameType不符，请检查您是否在导出之前忘记合并到提取出的模型上了？如果没有请合并到提取出的数据类型上以获取正确的3Dmigoto属性。");
            }
            LOG.Info(L"FMT文件解析验证完成");
            IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".ib", IBReadDxgiFormat);

            M_DrawIndexed m_drawIndexed;
            m_drawIndexed.DrawNumber = std::to_wstring(IBBufFile.NumberCount);
            m_drawIndexed.DrawOffsetIndex = std::to_wstring(Offset);
            m_drawIndexed.DrawStartIndex = L"0";

            PartName_DrawIndexed_Map[partName] = m_drawIndexed;

            Offset += IBBufFile.NumberCount;
        }
        LOG.NewLine();
        return PartName_DrawIndexed_Map;
    }



    std::unordered_map<std::string, M_DrawIndexed> Get_ToggleName_DrawIndexed_Map(DrawIBConfig IBConfig) {
        std::unordered_map<std::string, M_DrawIndexed> ToggleName_DrawIndexed_Map;

        LOG.Info(L"读取DrawIndexed数值");

        std::string IBReadDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
        uint64_t Offset = 0;
        //读取ib文件,转换格式后直接写出

        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".fmt";
                FmtFile fmtFile(partNameFmtFileName);
                if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                    LOG.Error(L"当前读取的fmt文件: " + partNameFmtFileName + L"中的ElementName与当前提取用的数据类型的GameType不符，请检查您是否在导出之前忘记合并到提取出的模型上了？如果没有请合并到提取出的数据类型上以获取正确的3Dmigoto属性。");
                }
                LOG.Info(L"FMT文件解析验证完成");
                IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".ib", MMTString::ToWideString(IBReadDxgiFormat));

                M_DrawIndexed m_drawIndexed;
                m_drawIndexed.DrawNumber = std::to_wstring(IBBufFile.NumberCount);
                m_drawIndexed.DrawOffsetIndex = std::to_wstring(Offset);
                m_drawIndexed.DrawStartIndex = L"0";

                ToggleName_DrawIndexed_Map[ToggleName] = m_drawIndexed;

                Offset += IBBufFile.NumberCount;
            }
        }

        LOG.NewLine();
        return ToggleName_DrawIndexed_Map;
    }


    void CombineIBToIndexBufFile(DrawIBConfig IBConfig) {
        LOG.Info(L"开始输出IB Buffer文件");

        //先设置读取ib文件所使用的步长,从fmt文件中自动读取
        std::wstring readFormatFileName = IBConfig.BufferReadFolder + L"1.fmt";
        std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(readFormatFileName, L"format");
        LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);


        std::vector<uint64_t> TotalIndexBufferNumberList;

        uint64_t Offset = 0;
        //读取ib文件,转换格式后直接写出
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".fmt";
            FmtFile fmtFile(partNameFmtFileName);
            if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                LOG.Error(L"当前读取的fmt文件: " + partNameFmtFileName + L"中的ElementName与当前提取用的数据类型的GameType不符，请检查您是否在导出之前忘记合并到提取出的模型上了？如果没有请合并到提取出的数据类型上以获取正确的3Dmigoto属性。");
            }
            LOG.Info(L"FMT文件解析验证完成");
            IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".ib", IBReadDxgiFormat);
            IBBufFile.AddOffset(Offset);

            TotalIndexBufferNumberList.insert(TotalIndexBufferNumberList.end(), IBBufFile.NumberList.begin(), IBBufFile.NumberList.end());

            Offset += IBBufFile.UniqueVertexCount;
            LOG.Info(L"IB文件输出完成");
        }
        LOG.NewLine();


        IndexBufferBufFile OutputIBBufFile(TotalIndexBufferNumberList);
        std::wstring OutputIBFilePath = IBConfig.BufferOutputFolder + IBConfig.DrawIB + L"Index.buf";
        OutputIBBufFile.SaveToFile_UINT32(OutputIBFilePath, 0);
    }


    void CombineIBToIndexBufFile_Lv2(DrawIBConfig IBConfig) {
        LOG.Info(L"开始输出IB Buffer文件");

        //先设置读取ib文件所使用的步长,从fmt文件中自动读取
        std::string IBReadDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;


        std::vector<uint64_t> TotalIndexBufferNumberList;
        uint64_t Offset = 0;
        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {

                std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".fmt";
                FmtFile fmtFile(partNameFmtFileName);
                if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                    LOG.Error(L"当前读取的fmt文件: " + partNameFmtFileName + L"中的ElementName与当前提取用的数据类型的GameType不符，请检查您是否在导出之前忘记合并到提取出的模型上了？如果没有请合并到提取出的数据类型上以获取正确的3Dmigoto属性。");
                }
                LOG.Info(L"FMT文件解析验证完成");
                IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".ib", MMTString::ToWideString(IBReadDxgiFormat));
                IBBufFile.AddOffset(Offset);

                TotalIndexBufferNumberList.insert(TotalIndexBufferNumberList.end(), IBBufFile.NumberList.begin(), IBBufFile.NumberList.end());

                Offset += IBBufFile.UniqueVertexCount;
                LOG.Info(L"IB文件输出完成");

            }
        }
        LOG.NewLine();


        IndexBufferBufFile OutputIBBufFile(TotalIndexBufferNumberList);
        std::wstring OutputIBFilePath = IBConfig.BufferOutputFolder + IBConfig.DrawIB + L"Index.buf";
        OutputIBBufFile.SaveToFile_UINT32(OutputIBFilePath, 0);
    }



    uint64_t GetSum_DrawNumber_FromVBFiles_Lv2(DrawIBConfig IBConfig) {
        //读取vb文件，每个vb文件都按照category分开装载不同category的数据
        uint64_t TmpDrawNumber = 0;
        uint64_t SplitStride = IBConfig.d3d11GameType.getElementListStride(IBConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));

        for (const auto& pair: IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName: ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                    LOG.Error(L"未检测到当前处理部位: 对应的VB文件: " + IBConfig.BufferReadFolder + VBFileName + L" 请检查您是否导出了全部的部位的模型。");
                }
                uint64_t VBFileSize = MMTFile::GetFileSize(IBConfig.BufferReadFolder + VBFileName);
                uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
                TmpDrawNumber = TmpDrawNumber + (uint64_t)vbFileVertexNumber;
            }
        }

        LOG.Info(L"Set draw number to: " + std::to_wstring(TmpDrawNumber));
        return TmpDrawNumber;
    }


    std::unordered_map<std::wstring, std::vector<std::byte>> Read_FinalVBCategoryDataMap_Lv2(DrawIBConfig IBConfig) {
        std::unordered_map<std::string, std::unordered_map<std::wstring, std::vector<std::byte>>> ToggleName_VBCategoryDaytaMap;

        //读取vb文件，每个vb文件都按照category分开装载不同category的数据
        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                    LOG.Warning(L"未检测到当前处理部位: " + MMTString::ToWideString(ToggleName) + L" 对应的VB文件: " + IBConfig.BufferReadFolder + VBFileName + L" 请检查您是否导出了全部的部位的模型。");
                }
                LOG.Info(L"Processing VB file: " + VBFileName);
                VertexBufferBufFile vbBufFile(IBConfig.BufferReadFolder + VBFileName, IBConfig.d3d11GameType, IBConfig.TmpElementList);
                ToggleName_VBCategoryDaytaMap[ToggleName] = vbBufFile.CategoryVBDataMap;
            }
        }



        //读取，然后替换COLOR、TANGENT，然后按类型分割放到这个map里
        std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;


        LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
        //将partName_VBCategoryDaytaMap里的内容，放入finalVBCategoryDataMap中组合成一个，供后续使用
        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = ToggleName_VBCategoryDaytaMap[ToggleName];

                for (size_t i = 0; i < IBConfig.d3d11GameType.OrderedCategoryNameList.size(); ++i) {
                    const std::string& category = IBConfig.d3d11GameType.OrderedCategoryNameList[i];
                    std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString::ToWideString(category)];

                    std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                    finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

                }
            }
        }


        LOG.NewLine();
        return finalVBCategoryDataMap;
    }



    void ShareSourceModel_Lv2(DrawIBConfig IBConfig) {
        //先创建好分享模型文件的目录
        std::filesystem::create_directories(IBConfig.ModelFolder);


        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                std::wstring IBFileName = MMTString::ToWideString(ToggleName) + L".ib";
                std::wstring FmtFileName = MMTString::ToWideString(ToggleName) + L".fmt";
                //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + VBFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName);
                //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + IBFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName);
                //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + FmtFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName);

                //复制到模型文件夹
                std::filesystem::copy_file(IBConfig.BufferReadFolder + VBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy_file(IBConfig.BufferReadFolder + IBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy_file(IBConfig.BufferReadFolder + FmtFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName, std::filesystem::copy_options::overwrite_existing);

            }
        }
       

    }

}
