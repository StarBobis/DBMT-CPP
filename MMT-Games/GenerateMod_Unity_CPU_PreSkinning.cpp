#include "Functions_Unity_CPU_PreSkinning.h"


#include "GlobalConfigs.h"
#include "D3d11GameType.h"
#include "VertexBufferBufFile.h"
#include "MMTConstants.h"

#include "BufferUtils.h"

void Functions_Unity_CPU_PreSkinning::GenerateMod() {
    LOG.Info(L"CPU_PreSkinning GenerateMod:");
    SingleIniBuilder CPUIniBuilder;

    for (const auto& pair : this->DrawIB_ExtractConfig_Map) {
        std::wstring drawIB = pair.first;
        DrawIBConfig drawIBConfig = pair.second;
        drawIBConfig.GameType = MMTString::ToWideString(drawIBConfig.WorkGameType);
        D3D11GameType d3d11GameType = d3d11GameTypeLv2.GameTypeName_D3d11GameType_Map[drawIBConfig.WorkGameType];

        if (!drawIBConfig.InitializeGenerateMod(G.Path_OutputFolder, d3d11GameType)) {
            continue;
        }
        LOG.Info("D3d11GameType: " + d3d11GameType.GameType);


        //��ȡ���ƵĶ�������
        drawIBConfig.DrawNumber = BufferUtils::GetSum_DrawNumber_FromVBFiles(drawIBConfig);

        //��ȡVB�ļ��е�����
        drawIBConfig.FinalVBCategoryDataMap = BufferUtils::Read_FinalVBCategoryDataMap(drawIBConfig);

        //��ֵ����
        if (drawIBConfig.AverageNormalTANGENT) {
            BufferUtils::Unity_TANGENT_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, d3d11GameType);
        }
        if (drawIBConfig.AverageNormalCOLOR) {
            BufferUtils::Unity_COLOR_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        }

        //Unity��Ϸ��Ҫ��תTANGENT��W����
        BufferUtils::Unity_Reverse_NORMAL_TANGENT(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        BufferUtils::Unity_Reset_COLOR(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);

        BufferUtils::Read_Convert_Output_IBBufferFiles(drawIBConfig);
        BufferUtils::OutputCategoryBufferFiles(drawIBConfig);

        if (G.ShareSourceModel) {
            BufferUtils::ShareSourceModel(drawIBConfig);
        }
        //------------------------------------------------------------------------------------------------------------------
        //ƴ��Ҫ�������ͼ����Դ��������ʹ�� 
        if (!G.ForbidAutoTexture) {
            //���������ͻ����һ�ν������
            this->PartName_TextureSlotReplace_Map.clear();
            for (const auto& pair : drawIBConfig.PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace(pair.second);
                this->PartName_TextureSlotReplace_Map[pair.first] = slotReplace;
            }
        }

        //1.TextureOverride VB���֣�ֻ��ʹ��GPU-PreSkinningʱ��ֱ���滻hash��Ӧ��λ
        if (d3d11GameType.GPUPreSkinning) {
            LOG.Info(L"Start to output TextureOverrideVB section");
            M_IniSection TextureOverrideVBSection(IniSectionType::TextureOverrideVB);
            for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
                TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(categoryHash));
 

                //������ȡ�����ڵ�ǰ����hash�½����滻�ķ��࣬����Ӷ�Ӧ����Դ�滻
                for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                    std::string originalCategoryName = pair.first;
                    std::string drawCategoryName = pair.second;
                    if (categoryName == drawCategoryName) {
                        std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                        TextureOverrideVBSection.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                    }
                }

                //drawһ�㶼����Blend��λ�Ͻ��еģ�������������Ҫ�ж�ȷ����BlendҪ�滻��hash���ܽ���draw��
                if (categoryName == d3d11GameType.CategoryDrawCategoryMap["Blend"]) {
                    TextureOverrideVBSection.Append(L"handling = skip");
                    TextureOverrideVBSection.Append(L"draw = " + std::to_wstring(drawIBConfig.DrawNumber) + L", 0");
                }

                TextureOverrideVBSection.NewLine();
            }

            //2.VertexLimitRaise���֣�����ֻ��ʹ�õ�GPU-PreSkinning����ʱ����Ҫͻ�ƶ�����������
            TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_" + std::to_wstring(d3d11GameType.CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawIBConfig.DrawNumber) + L"_VertexLimitRaise]");
            TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(drawIBConfig.VertexLimitVB));
            TextureOverrideVBSection.NewLine();

            CPUIniBuilder.AppendSection(TextureOverrideVBSection);
        }

        //1.IB SKIP����
        M_IniSection IBSkipSection(IniSectionType::IBSkip);
        IBSkipSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"IB]");
        IBSkipSection.Append(L"hash = " + drawIBConfig.DrawIB);
        IBSkipSection.Append(L"handling = skip");
        IBSkipSection.NewLine();
        CPUIniBuilder.AppendSection(IBSkipSection);

        //2.IBOverride����

        M_IniSection TextureOverrideIBSection(IniSectionType::TextureOverrideIB);
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string IBFirstIndex = drawIBConfig.MatchFirstIndexList[i];
            std::string partName = drawIBConfig.PartNameList[i];

            std::wstring IBResourceName = L"Resource" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName));

            TextureOverrideIBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L"]");
            TextureOverrideIBSection.Append(L"hash = " + drawIBConfig.DrawIB);
            TextureOverrideIBSection.Append(L"match_first_index = " + MMTString::ToWideString(IBFirstIndex));


            //�滻IB��λ
            TextureOverrideIBSection.Append(L"ib = " + IBResourceName);

            //��ͼ��Դ�滻
            std::vector<std::wstring> slotReplaceStrList = this->PartName_TextureSlotReplace_Map[partName].Out_SlotReplaceStrList;

            if (!G.ForbidAutoTexture && !G.UseHashTexture) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    TextureOverrideIBSection.Append(slotReplaceStr);
                }
            }


            //�����ʹ��GPU-Skinning��ΪObject���ͣ���ʱ��Ҫ��ib�����滻��Ӧ��λ
            if (!d3d11GameType.GPUPreSkinning) {
                for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                    std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                    std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                    LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));

                    //������ȡ�����ڵ�ǰ����hash�½����滻�ķ��࣬����Ӷ�Ӧ����Դ�滻
                    for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                        std::string originalCategoryName = pair.first;
                        std::string drawCategoryName = pair.second;
                        if (categoryName == drawCategoryName) {
                            std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                            TextureOverrideIBSection.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                        }
                    }
                }
            }

            //Drawͼ��
            TextureOverrideIBSection.Append(L"drawindexed = auto");
            TextureOverrideIBSection.NewLine();
        }
        CPUIniBuilder.AppendSection(TextureOverrideIBSection);


        //5.д��VBResource����
        M_IniSection ResourceVBSection(IniSectionType::ResourceVB);
        for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
            ResourceVBSection.Append(L"[Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
            ResourceVBSection.Append(L"type = Buffer");

            if (categoryName == "Blend" && d3d11GameType.PatchBLENDWEIGHTS) {
                uint64_t finalBlendStride = d3d11GameType.CategoryStrideMap[categoryName] - d3d11GameType.ElementNameD3D11ElementMap["BLENDWEIGHT"].ByteWidth;
                ResourceVBSection.Append(L"stride = " + std::to_wstring(finalBlendStride));

            }
            else {
                ResourceVBSection.Append(L"stride = " + std::to_wstring(d3d11GameType.CategoryStrideMap[categoryName]));

            }
            //vb�ļ����ļ���
            ResourceVBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf");
            ResourceVBSection.NewLine();
        }
        CPUIniBuilder.AppendSection(ResourceVBSection);


        //6.д��IBResource����
        M_IniSection ResourceIBSection(IniSectionType::ResourceIB);
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string partName = drawIBConfig.PartNameList[i];
            ResourceIBSection.Append(L"[Resource" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L"]");
            ResourceIBSection.Append(L"type = Buffer");
            ResourceIBSection.Append(L"format = DXGI_FORMAT_R32_UINT");
            ResourceIBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L".ib");
            ResourceIBSection.NewLine();
        }
        CPUIniBuilder.AppendSection(ResourceIBSection);


        //7.д����ͼresource����
        if (!G.ForbidAutoTexture && !G.UseHashTexture) {
            M_IniSection ResourceTextureSection(IniSectionType::ResourceTexture);
            for (const auto& pair : this->PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace = pair.second;
                for (const auto& slotPair : slotReplace.Out_ResourceName_FileName_Map) {
                    std::wstring ResourceName = slotPair.first;
                    std::wstring ResourceFileName = slotPair.second;
                    ResourceTextureSection.Append(L"[" + ResourceName + L"]");
                    ResourceTextureSection.Append(L"filename = Texture/" + ResourceFileName);
                    ResourceTextureSection.NewLine();
                }
            }
            CPUIniBuilder.AppendSection(ResourceTextureSection);
        }


        //д��ر��ļ�
        LOG.NewLine();

        //�ƶ���λ��DDS�ļ���Mod�ļ�����
        BufferUtils::CopySlotDDSTextureFromOutputFolder(drawIBConfig);

        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

    }


    if (!std::filesystem::exists(G.ModOutputFolder)) {
        LOG.Error(MMT_Tips::TIP_GenerateMod_NoAnyExportModelDetected);
    }
    else {
        LOG.Info(L"��ʼ���ɻ���Hashֵ��this = ������ͼ�滻,��δ��ѡ�򲻻�����");
        std::unordered_map<std::wstring, std::wstring> Hash_TextureFileNameMap;
        for (const auto& DrawIBPair : this->DrawIB_ExtractConfig_Map) {
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

            for (const auto& DrawIBPair : this->DrawIB_ExtractConfig_Map) {
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
                        //��ƴ��д��Resource

                        M_IniSection ResourceSection(IniSectionType::ResourceTexture);
                        ResourceSection.SectionLineList.push_back(L"[Resource_Texture_" + TextureHash + L"]");
                        ResourceSection.SectionLineList.push_back(L"filename = Texture/" + NewFileName + L"");
                        ResourceSection.NewLine();
                        CPUIniBuilder.AppendSection(ResourceSection);

                        M_IniSection TextureOverrideSection(IniSectionType::TextureOverrideTexture);
                        TextureOverrideSection.SectionLineList.push_back(L"[TextureOverride_" + TextureHash + L"]");
                        TextureOverrideSection.SectionLineList.push_back(L"hash = " + TextureHash);
                        TextureOverrideSection.SectionLineList.push_back(L"this = Resource_Texture_" + TextureHash);
                        TextureOverrideSection.NewLine();
                        CPUIniBuilder.AppendSection(TextureOverrideSection);

                        //��ԭ�����ļ����Ƶ���Ӧ��Hash�ļ�
                        std::filesystem::copy_file(FilePath, TextureOutputFolder + NewFileName, std::filesystem::copy_options::skip_existing);
                    }
                }
                LOG.NewLine();
            }

            LOG.Info(L"����Hash��ͼ�滻�ɹ�");
        }
        LOG.NewLine();

    }

    CPUIniBuilder.AppendSection(BufferUtils::GetCreditInfoIniSection());
    CPUIniBuilder.SaveToFile(G.ModOutputFolder + L"Config.ini");

    BufferUtils::WriteCreditREADME();

}