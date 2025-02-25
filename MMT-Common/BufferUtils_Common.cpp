#include "BufferUtils.h"

#include "VertexBufferBufFile.h"
#include "FmtFile.h"

namespace BufferUtils {

    void WriteHashStyleTextureIni_HI3(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map) {
        if (!std::filesystem::exists(G.ModOutputFolder)) {
            LOG.Error(MMT_Tips::TIP_GenerateMod_NoAnyExportModelDetected);
        }

        LOG.Info(L"写出Hash风格贴图ini");
        //两个DrawIB可能会用到重复hash的贴图，也就是同一张贴图，这时候让他们共用一张贴图。
        std::unordered_map<std::wstring, std::wstring> Total_Hash_TextureFileName_Map;
        for (const auto& pair : DrawIB_ExtractConfig_Map) {
            std::wstring drawIB = pair.first;
            DrawIBConfig drawIBConfig = pair.second;
            for (const auto& TexturePair: drawIBConfig.Hash_TextureFileName_Map) {
                Total_Hash_TextureFileName_Map[MMTString::ToWideString(TexturePair.first)] = MMTString::ToWideString(TexturePair.second);
            }
        }

        LOG.Info(L"Hash_TextureFileNameMap Size: " + std::to_wstring(Total_Hash_TextureFileName_Map.size()));
        LOG.NewLine();
        if (Total_Hash_TextureFileName_Map.size() != 0) {
            std::wstring TextureOutputFolder = G.ModOutputFolder + L"Texture\\";
            std::filesystem::create_directories(TextureOutputFolder);

            SingleIniBuilder TextureIniBuilder;
            for (const auto& TexturePair : Total_Hash_TextureFileName_Map) {
                std::wstring TextureHash = TexturePair.first;
                std::wstring FileName = TexturePair.second;

                std::wstring FilePath = G.DedupedFolder + FileName;
                LOG.Info(L"Current Processing: " + FilePath);

                if (std::filesystem::exists(FilePath)) {
                    //先拼接写出Resource

                    M_IniSection ResourceSection(IniSectionType::ResourceTexture);
                    ResourceSection.SectionLineList.push_back(L"[Resource_Texture_" + TextureHash + L"]");
                    ResourceSection.SectionLineList.push_back(L"filename = Texture/" + FileName + L"");
                    ResourceSection.NewLine();
                    TextureIniBuilder.AppendSection(ResourceSection);

                    M_IniSection TextureOverrideSection(IniSectionType::TextureOverrideTexture);
                    TextureOverrideSection.SectionLineList.push_back(L"[TextureOverride_" + TextureHash + L"]");
                    TextureOverrideSection.SectionLineList.push_back(L"hash = " + TextureHash);
                    TextureOverrideSection.SectionLineList.push_back(L"this = Resource_Texture_" + TextureHash);
                    TextureOverrideSection.NewLine();
                    TextureIniBuilder.AppendSection(TextureOverrideSection);

                    //把原本的文件复制到对应的Hash文件
                    std::filesystem::copy_file(FilePath, TextureOutputFolder + FileName, std::filesystem::copy_options::skip_existing);
                }
            }

            TextureIniBuilder.SaveToFile(G.ModOutputFolder + L"TextureReplace.ini");

            LOG.Info(L"生成Hash贴图替换成功");
        }
        LOG.NewLine();
    }



    M_IniSection GetCreditInfoIniSection() {
        M_IniSection CreditInfoSection(IniSectionType::CreditInfo);
        CreditInfoSection.Append(L"; Author: " + G.Author + L" , All Rights Reserved.");
        CreditInfoSection.Append(L"; Sponser Link: " + G.AuthorLink);
        //不需要日期，不然作者岂不是不能有存货了，自古真情留不住，唯有套路得人心。
        //CreditInfoSection.Append(L"; Create Date: " + MMTString::GetFormattedDateTimeForFilename());
        CreditInfoSection.Append(L"; Mod Files Provided With Default LICENSE: https://creativecommons.org/licenses/by-nc-nd/4.0/deed.en");
        CreditInfoSection.NewLine();

        //CreditInfoSection.Append(L"; COPYRIGHTS WARNING: ");
        //CreditInfoSection.Append(L"; Mod generated with MMT not allowed to be reverse extract model by default! ");
        //CreditInfoSection.Append(L"; If author agree to share their model, there will be a \"Model\" folder under your mod folder.");
        //CreditInfoSection.Append(L"; Inside it will be the source model of this mod if author check ShareSourceModel option in MMT Setting.");
        //CreditInfoSection.Append(L"; Please report back to the author if you find someone abuse his/her/... model without permission, thanks for your contribution for the good future of mod community.");
        //CreditInfoSection.NewLine();
        
        //CreditInfoSection.Append(L"; 3Dmigoto Mod auto generated by MigotoModTool(MMT).");
        //CreditInfoSection.Append(L"; Open an issue if you have any problem or advice: https://github.com/StarBobis/MigotoModTool");
        //CreditInfoSection.NewLine();
        return CreditInfoSection;
    }
    

    void WriteCreditREADME() {
        LOG.Info(L"写出README.txt");
        SingleIniBuilder CreditInfoTxtBuilder;
        M_IniSection CreditInfoSection = BufferUtils::GetCreditInfoIniSection();
        CreditInfoTxtBuilder.AppendSection(CreditInfoSection);
        CreditInfoTxtBuilder.SaveToFile(G.ModOutputFolder + L"README.txt");
        LOG.NewLine();
    }


    void ShareSourceModel(DrawIBConfig IBConfig) {
        //先创建好分享模型文件的目录
        std::filesystem::create_directories(IBConfig.ModelFolder);

        //把原始的模型文件复制到Model目录中
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring VBFileName =  MMTString::ToWideString(partName) + L".vb";
            std::wstring IBFileName = MMTString::ToWideString(partName) + L".ib";
            std::wstring FmtFileName =  MMTString::ToWideString(partName) + L".fmt";
            //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + VBFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName);
            //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + IBFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName);
            //LOG.Info(L"分享源模型:" + IBConfig.BufferReadFolder + FmtFileName + L" 到:" + IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName);
            
            //复制到模型文件夹
            std::filesystem::copy_file(IBConfig.BufferReadFolder + VBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + VBFileName, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy_file(IBConfig.BufferReadFolder + IBFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + IBFileName, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy_file(IBConfig.BufferReadFolder + FmtFileName, IBConfig.ModelFolder + IBConfig.DrawIB + L"_" + FmtFileName, std::filesystem::copy_options::overwrite_existing);
        }

    }


    uint64_t GetSum_DrawNumber_FromVBFiles( DrawIBConfig IBConfig) {
        //读取vb文件，每个vb文件都按照category分开装载不同category的数据
        uint64_t TmpDrawNumber = 0;
        uint64_t SplitStride = IBConfig.d3d11GameType.getElementListStride(IBConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
            if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                LOG.Error(L"未检测到当前处理部位: " + MMTString::ToWideString(partName) + L" 对应的VB文件: " + IBConfig.BufferReadFolder + VBFileName + L" 请检查您是否导出了全部的部位的模型。");
            }
            uint64_t VBFileSize = MMTFile::GetFileSize(IBConfig.BufferReadFolder + VBFileName);
            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
            TmpDrawNumber = TmpDrawNumber + (uint64_t)vbFileVertexNumber;
        }
        LOG.Info(L"Set draw number to: " + std::to_wstring(TmpDrawNumber));
        return TmpDrawNumber;
    }


    std::unordered_map<std::wstring, std::vector<std::byte>> Read_FinalVBCategoryDataMap( DrawIBConfig IBConfig) {
        std::unordered_map<std::string, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;

        //读取vb文件，每个vb文件都按照category分开装载不同category的数据
        for (std::string partName : IBConfig.PartNameList) {
            std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
            if (!std::filesystem::exists(IBConfig.BufferReadFolder + VBFileName)) {
                LOG.Warning(L"未检测到当前处理部位: " + MMTString::ToWideString(partName) + L" 对应的VB文件: " + IBConfig.BufferReadFolder + VBFileName + L" 请检查您是否导出了全部的部位的模型。");
            }
            LOG.Info(L"Processing VB file: " + VBFileName);
            VertexBufferBufFile vbBufFile(IBConfig.BufferReadFolder + VBFileName, IBConfig.d3d11GameType, IBConfig.TmpElementList);
            partName_VBCategoryDaytaMap[partName] = vbBufFile.CategoryVBDataMap;
        }


        //读取，然后替换COLOR、TANGENT，然后按类型分割放到这个map里
        std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;


        LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
        //将partName_VBCategoryDaytaMap里的内容，放入finalVBCategoryDataMap中组合成一个，供后续使用
        for (std::string partName : IBConfig.PartNameList) {
            std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[partName];

            for (size_t i = 0; i < IBConfig.d3d11GameType.OrderedCategoryNameList.size(); ++i) {
                const std::string& category = IBConfig.d3d11GameType.OrderedCategoryNameList[i];
                std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString::ToWideString(category)];

                std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

            }
        }


        LOG.NewLine();
        return finalVBCategoryDataMap;
    }


    //TODO 后面移动到TexturesUtils里
    void CopySlotDDSTextureFromOutputFolder( DrawIBConfig IBConfig) {
        //仅在使用槽位的自动贴图中使用。
        //LOG.Info("Start to copy dds textures from our output folder.");
        //这里不是读取所有dds文件夹复制，而是从tmp.json中找文件名复制
        if (!G.ForbidAutoTexture && !G.UseHashTexture) {
            for (const auto& pair : IBConfig.PartName_TextureSlotReplace_Map) {
                for (std::string textureFileName : pair.second) {
                    size_t equalPos = textureFileName.find_first_of("=");
                    std::string textureFileNameNew = textureFileName.substr(equalPos + 1);
                    boost::algorithm::trim(textureFileNameNew);
                    //LOG.Info(L"准备复制：" + MMTString::ToWideString(textureFileNameNew));
                    if (!std::filesystem::exists(IBConfig.BufferReadFolder + MMTString::ToWideString(textureFileNameNew))) {
                        //LOG.Info(L"未找到该文件，跳过复制移动");
                        continue;
                    }
                    std::filesystem::copy_file(IBConfig.BufferReadFolder + MMTString::ToWideString(textureFileNameNew), IBConfig.TextureOutputFolder + MMTString::ToWideString(textureFileNameNew), std::filesystem::copy_options::skip_existing);
                }
            }
        }
        LOG.NewLine();
    }





    //Deprecated 旧的格式已过时，在新格式测试完成后删除
    void Read_Convert_Output_IBBufferFiles( DrawIBConfig IBConfig) {
        LOG.Info(L"开始输出IB Buffer文件");

        //先设置读取ib文件所使用的步长,从fmt文件中自动读取
        //读取body_part0.fmt文件中的Format
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
            IBBufFile.SaveToFile_UINT32(IBConfig.BufferOutputFolder + IBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L".ib", Offset);
            Offset += IBBufFile.UniqueVertexCount;
            LOG.Info(L"IB文件输出完成");
        }
        LOG.NewLine();
    }


    void OutputCategoryBufferFiles(DrawIBConfig IBConfig) {
        LOG.Info(L"开始输出Category Buffer文件");
        std::string blendElementName;
        if (boost::algorithm::any_of_equal(IBConfig.TmpElementList, "BLENDWEIGHT")) {
            blendElementName = "BLENDWEIGHT";
        }
        else {
            blendElementName = "BLENDWEIGHTS";
        }
        uint64_t blendElementByteWidth = IBConfig.d3d11GameType.ElementNameD3D11ElementMap[blendElementName].ByteWidth;
        uint64_t blendIndicesByteWidth = IBConfig.d3d11GameType.ElementNameD3D11ElementMap["BLENDINDICES"].ByteWidth;
        LOG.Info(L"Get BlendWeigths width: " + std::to_wstring(blendElementByteWidth));
        LOG.Info(L"Get BlendIndices width: " + std::to_wstring(blendIndicesByteWidth));

        for (const auto& pair : IBConfig.FinalVBCategoryDataMap) {
            const std::wstring& categoryName = pair.first;
            const std::vector<std::byte>& categoryData = pair.second;
            LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / IBConfig.DrawNumber));
            //如果没有那就不输出
            if (categoryData.size() == 0) {
                LOG.Info(L"Current category's size is 0, can't output, skip this.");
                continue;
            }

            // 构建输出文件路径
            std::wstring outputDatFilePath = IBConfig.BufferOutputFolder + IBConfig.DrawIB + categoryName + L".buf";
            // 打开输出文件
            std::ofstream outputFile(MMTString::ToByteString(outputDatFilePath), std::ios::binary);

            //如果缺失BLENDWEIGHTS元素，则需要手动删除BLEND槽位中的BLEND元素
            if (IBConfig.d3d11GameType.PatchBLENDWEIGHTS && categoryName == L"Blend") {

                LOG.Info("Detect element: " + blendElementName + " need to delete , now will delete it.");
                //去除BLENDWEIGHTS后的数据列表
                std::vector<std::byte> newBlendCategoryData = MMTFormat::Remove_BLENDWEIGHTS(categoryData, blendElementByteWidth, blendIndicesByteWidth);
                outputFile.write(reinterpret_cast<const char*>(newBlendCategoryData.data()), newBlendCategoryData.size());
            }
            else {
                // 将std::vecto的内容写入文件
                outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
            }
            outputFile.close();

            LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
        }
        LOG.NewLine();
    }

    std::vector<M_IniSection> GenerateHashStyleTextureIni(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map) {

        std::vector<M_IniSection> TextureIniSectionList;

        LOG.Info(L"开始生成基于Hash值的this = 类型贴图替换,如未勾选则不会生成");
        std::unordered_map<std::wstring, std::wstring> Hash_TextureFileNameMap;
        for (const auto& DrawIBPair : DrawIB_ExtractConfig_Map) {
            std::wstring drawIB = DrawIBPair.first;
            DrawIBConfig extractConfig = DrawIBPair.second;
            LOG.Info(L"DrawIB: " + drawIB);
            LOG.Info(L"UseHashTexture: " + std::to_wstring(G.UseHashTexture));
            LOG.Info(L"Allow TextureGenerate: " + std::to_wstring(!G.ForbidAutoTexture));
            LOG.NewLine();
            if (G.UseHashTexture && !G.ForbidAutoTexture) {
                for (const auto& texturePair : extractConfig.PartName_TextureSlotReplace_Map) {
                    std::string PartName = texturePair.first;
                    std::vector<std::string> TextureFileNameList = texturePair.second;
                    for (std::string TextureFileName : TextureFileNameList) {

                        std::vector<std::wstring> TextureFileNameSplitList = MMTString::SplitStringOnlyMatchFirst(MMTString::ToWideString(TextureFileName), L"=");
                        std::wstring RealTextureFileName = boost::algorithm::trim_copy(TextureFileNameSplitList[1]);
                        std::vector<std::wstring> StrSplitList = MMTString::SplitString(RealTextureFileName, L"-");
                        //std::wstring DrawIB = StrSplitList[0];
                        std::wstring TextureHash = StrSplitList[1];
                        Hash_TextureFileNameMap[TextureHash] = RealTextureFileName;
                    }
                }
            }
        }
        LOG.Info(L"Hash_TextureFileNameMap Size: " + std::to_wstring(Hash_TextureFileNameMap.size()));
        LOG.NewLine();


        if (Hash_TextureFileNameMap.size() != 0) {
            std::wstring TextureOutputFolder = G.ModOutputFolder + L"Texture\\";
            std::filesystem::create_directories(TextureOutputFolder);

            for (const auto& DrawIBPair : DrawIB_ExtractConfig_Map) {
                std::wstring drawIB = DrawIBPair.first;
                DrawIBConfig extractConfig = DrawIBPair.second;
                std::wstring BufferReadFolder = G.Path_OutputFolder + drawIB + L"/";
                LOG.Info(L"DrawIB: " + drawIB);

                for (const auto& TexturePair : Hash_TextureFileNameMap) {
                    std::wstring TextureHash = TexturePair.first;
                    std::wstring FileName = TexturePair.second;

                    std::wstring FilePath = BufferReadFolder + FileName;
                    LOG.Info(L"Current Processing: " + FilePath);

                    std::vector<std::wstring> StrSplitList = MMTString::SplitString(FileName, L"-");
                    std::wstring NewFileName = drawIB + L"_" + TextureHash + L"_" + StrSplitList[3];

                    if (std::filesystem::exists(FilePath)) {
                        //先拼接写出Resource

                        M_IniSection ResourceSection(IniSectionType::ResourceTexture);
                        ResourceSection.SectionLineList.push_back(L"[Resource_Texture_" + TextureHash + L"]");
                        ResourceSection.SectionLineList.push_back(L"filename = Texture/" + NewFileName + L"");
                        ResourceSection.NewLine();
                        TextureIniSectionList.push_back(ResourceSection);

                        M_IniSection TextureOverrideSection(IniSectionType::TextureOverrideTexture);
                        TextureOverrideSection.SectionLineList.push_back(L"[TextureOverride_" + TextureHash + L"]");
                        TextureOverrideSection.SectionLineList.push_back(L"hash = " + TextureHash);
                        TextureOverrideSection.SectionLineList.push_back(L"this = Resource_Texture_" + TextureHash);
                        TextureOverrideSection.NewLine();
                        TextureIniSectionList.push_back(TextureOverrideSection);

                        //把原本的文件复制到对应的Hash文件
                        std::filesystem::copy_file(FilePath, TextureOutputFolder + NewFileName, std::filesystem::copy_options::skip_existing);
                    }
                }
                LOG.NewLine();
            }

            LOG.Info(L"生成Hash贴图替换成功");
        }
        LOG.NewLine();

        return TextureIniSectionList;
    }


}