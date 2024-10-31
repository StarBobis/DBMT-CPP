#include "BufferUtils.h"

#include "VertexBufferBufFile.h"
#include "FmtFile.h"
#include "MMTConstants.h"

namespace BufferUtilsLv2 {
    //PartName_DrawIndexed_Map 
    //Deprecated
    //��չ��ÿ��PartName�����ж����λ���¼ܹ�֮��������Ƴ�����
    std::unordered_map<std::string, M_DrawIndexed> Get_PartName_DrawIndexed_Map(DrawIBConfig IBConfig) {
        std::unordered_map<std::string, M_DrawIndexed> PartName_DrawIndexed_Map;

        LOG.Info(L"��ȡDrawIndexed��ֵ");

        std::wstring readFormatFileName = IBConfig.BufferReadFolder + L"1.fmt";
        std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(readFormatFileName, L"format");
        LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);
        uint64_t Offset = 0;
        //��ȡib�ļ�,ת����ʽ��ֱ��д��
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".fmt";
            FmtFile fmtFile(partNameFmtFileName);
            if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                LOG.Error(L"��ǰ��ȡ��fmt�ļ�: " + partNameFmtFileName + L"�е�ElementName�뵱ǰ��ȡ�õ��������͵�GameType�������������Ƿ��ڵ���֮ǰ���Ǻϲ�����ȡ����ģ�����ˣ����û����ϲ�����ȡ���������������Ի�ȡ��ȷ��3Dmigoto���ԡ�");
            }
            LOG.Info(L"FMT�ļ�������֤���");
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

        LOG.Info(L"��ȡDrawIndexed��ֵ");

        std::string IBReadDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
        uint64_t Offset = 0;
        //��ȡib�ļ�,ת����ʽ��ֱ��д��

        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".fmt";
                FmtFile fmtFile(partNameFmtFileName);
                if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                    LOG.Error(L"��ǰ��ȡ��fmt�ļ�: " + partNameFmtFileName + L"�е�ElementName�뵱ǰ��ȡ�õ��������͵�GameType�������������Ƿ��ڵ���֮ǰ���Ǻϲ�����ȡ����ģ�����ˣ����û����ϲ�����ȡ���������������Ի�ȡ��ȷ��3Dmigoto���ԡ�");
                }
                LOG.Info(L"FMT�ļ�������֤���");
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
        LOG.Info(L"��ʼ���IB Buffer�ļ�");

        //�����ö�ȡib�ļ���ʹ�õĲ���,��fmt�ļ����Զ���ȡ
        std::wstring readFormatFileName = IBConfig.BufferReadFolder + L"1.fmt";
        std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(readFormatFileName, L"format");
        LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);


        std::vector<uint64_t> TotalIndexBufferNumberList;

        uint64_t Offset = 0;
        //��ȡib�ļ�,ת����ʽ��ֱ��д��
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring partNameFmtFileName = IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".fmt";
            FmtFile fmtFile(partNameFmtFileName);
            if (!fmtFile.IsD3d11ElementListMatch(IBConfig.d3d11GameType.OrderedFullElementList)) {
                LOG.Error(L"��ǰ��ȡ��fmt�ļ�: " + partNameFmtFileName + L"�е�ElementName�뵱ǰ��ȡ�õ��������͵�GameType�������������Ƿ��ڵ���֮ǰ���Ǻϲ�����ȡ����ģ�����ˣ����û����ϲ�����ȡ���������������Ի�ȡ��ȷ��3Dmigoto���ԡ�");
            }
            LOG.Info(L"FMT�ļ�������֤���");
            IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(partName) + L".ib", IBReadDxgiFormat);
            IBBufFile.AddOffset(Offset);

            TotalIndexBufferNumberList.insert(TotalIndexBufferNumberList.end(), IBBufFile.NumberList.begin(), IBBufFile.NumberList.end());

            Offset += IBBufFile.UniqueVertexCount;
            LOG.Info(L"IB�ļ�������");
        }
        LOG.NewLine();


        IndexBufferBufFile OutputIBBufFile(TotalIndexBufferNumberList);
        std::wstring OutputIBFilePath = IBConfig.BufferOutputFolder + IBConfig.DrawIB + L"Index.buf";
        OutputIBBufFile.SaveToFile_UINT32(OutputIBFilePath, 0);
    }


    void CombineIBToIndexBufFile_Lv2(DrawIBConfig IBConfig) {
        LOG.Info(L"��ʼ���IB Buffer�ļ�");

        //�����ö�ȡib�ļ���ʹ�õĲ���,��fmt�ļ����Զ���ȡ
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
                    LOG.Error(L"��ǰ��ȡ��fmt�ļ�: " + partNameFmtFileName + L"�е�ElementName�뵱ǰ��ȡ�õ��������͵�GameType�������������Ƿ��ڵ���֮ǰ���Ǻϲ�����ȡ����ģ�����ˣ����û����ϲ�����ȡ���������������Ի�ȡ��ȷ��3Dmigoto���ԡ�");
                }
                LOG.Info(L"FMT�ļ�������֤���");
                IndexBufferBufFile IBBufFile(IBConfig.BufferReadFolder + MMTString::ToWideString(ToggleName) + L".ib", MMTString::ToWideString(IBReadDxgiFormat));
                IBBufFile.AddOffset(Offset);

                TotalIndexBufferNumberList.insert(TotalIndexBufferNumberList.end(), IBBufFile.NumberList.begin(), IBBufFile.NumberList.end());

                Offset += IBBufFile.UniqueVertexCount;
                LOG.Info(L"IB�ļ�������");

            }
        }
        LOG.NewLine();


        IndexBufferBufFile OutputIBBufFile(TotalIndexBufferNumberList);
        std::wstring OutputIBFilePath = IBConfig.BufferOutputFolder + IBConfig.DrawIB + L"Index.buf";
        OutputIBBufFile.SaveToFile_UINT32(OutputIBFilePath, 0);
    }



    uint64_t GetSum_DrawNumber_FromVBFiles_Lv2(DrawIBConfig IBConfig) {
        //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
        uint64_t TmpDrawNumber = 0;
        uint64_t SplitStride = IBConfig.d3d11GameType.getElementListStride(IBConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));

        for (const auto& pair: IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName: ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                    LOG.Error(L"δ��⵽��ǰ����λ: ��Ӧ��VB�ļ�: " + IBConfig.BufferReadFolder + VBFileName + L" �������Ƿ񵼳���ȫ���Ĳ�λ��ģ�͡�");
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

        //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                    LOG.Warning(L"δ��⵽��ǰ����λ: " + MMTString::ToWideString(ToggleName) + L" ��Ӧ��VB�ļ�: " + IBConfig.BufferReadFolder + VBFileName + L" �������Ƿ񵼳���ȫ���Ĳ�λ��ģ�͡�");
                }
                LOG.Info(L"Processing VB file: " + VBFileName);
                VertexBufferBufFile vbBufFile(IBConfig.BufferReadFolder + VBFileName, IBConfig.d3d11GameType, IBConfig.TmpElementList);
                ToggleName_VBCategoryDaytaMap[ToggleName] = vbBufFile.CategoryVBDataMap;
            }
        }



        //��ȡ��Ȼ���滻COLOR��TANGENT��Ȼ�����ͷָ�ŵ����map��
        std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;


        LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
        //��partName_VBCategoryDaytaMap������ݣ�����finalVBCategoryDataMap����ϳ�һ����������ʹ��
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
        //�ȴ����÷���ģ���ļ���Ŀ¼
        std::filesystem::create_directories(IBConfig.ModelFolder);


        for (const auto& pair : IBConfig.ModelPrefixName_ToggleNameList_Map) {
            std::string ModelPrefixName = pair.first;
            std::vector<std::string> ToggleNameList = pair.second;
            for (std::string ToggleName : ToggleNameList) {
                std::wstring VBFileName = MMTString::ToWideString(ToggleName) + L".vb";
                std::wstring IBFileName = MMTString::ToWideString(ToggleName) + L".ib";
                std::wstring FmtFileName = MMTString::ToWideString(ToggleName) + L".fmt";
                //LOG.Info(L"����Դģ��:" + IBConfig.BufferReadFolder + VBFileName + L" ��:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName);
                //LOG.Info(L"����Դģ��:" + IBConfig.BufferReadFolder + IBFileName + L" ��:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName);
                //LOG.Info(L"����Դģ��:" + IBConfig.BufferReadFolder + FmtFileName + L" ��:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName);

                //���Ƶ�ģ���ļ���
                std::filesystem::copy_file(IBConfig.BufferReadFolder + VBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy_file(IBConfig.BufferReadFolder + IBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy_file(IBConfig.BufferReadFolder + FmtFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName, std::filesystem::copy_options::overwrite_existing);

            }
        }
       

    }

}
