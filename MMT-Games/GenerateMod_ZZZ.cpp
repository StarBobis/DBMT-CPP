#include "Functions_ZZZ.h"

#include "GlobalConfigs.h"
#include "D3d11GameType.h"
#include "VertexBufferBufFile.h"
#include "MMTConstants.h"

#include "BufferUtils.h"
#include "MigotoIniBuilder.h"


void Functions_ZZZ::GenerateMod() {

    //��һ��IniBuilder.AppendLine();
    LOG.Info("Executing: GenerateMod_ZZZ");
    
    SingleIniBuilder ZZZIniBuilder;

    //��Constants��Present��Key������ʱ�õ�ͳ�Ʊ���
    uint64_t GlobalKeyIndex_Constants = 0;
    //��ini����ʵ����ʱ�õ�ͳ�Ʊ���
    uint64_t GlobalKeyIndex_Logic = 0;
    //ÿ��Mod������active��������1
    uint64_t GlobalActiveIndex = 0;

    for (const auto& DrawIBPair : DrawIB_ExtractConfig_Map) {
        std::wstring drawIB = DrawIBPair.first;
        DrawIBConfig drawIBConfig = DrawIBPair.second;
        drawIBConfig.GameType = MMTString::ToWideString(drawIBConfig.WorkGameType);
        D3D11GameType d3d11GameType = d3d11GameTypeLv2.GameTypeName_D3d11GameType_Map[drawIBConfig.WorkGameType];

        if (!drawIBConfig.InitializeGenerateMod(G.Path_OutputFolder, d3d11GameType)) {
            continue;
        }
        LOG.Info("D3d11GameType: " + d3d11GameType.GameType);


        //��ȡ���ƵĶ�������
        drawIBConfig.DrawNumber = BufferUtilsLv2::GetSum_DrawNumber_FromVBFiles_Lv2(drawIBConfig);

        //��ȡVB�ļ��е�����
        drawIBConfig.FinalVBCategoryDataMap = BufferUtilsLv2::Read_FinalVBCategoryDataMap_Lv2(drawIBConfig);

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

        BufferUtilsLv2::CombineIBToIndexBufFile_Lv2(drawIBConfig);
        std::unordered_map<std::string, M_DrawIndexed> ToggleName_DrawIndexed_Map = BufferUtilsLv2::Get_ToggleName_DrawIndexed_Map(drawIBConfig);

        BufferUtils::OutputCategoryBufferFiles(drawIBConfig);

        if (G.ShareSourceModel) {
            BufferUtilsLv2::ShareSourceModel_Lv2(drawIBConfig);
        }

        //��3����������Wheel��ʽ��ini�ļ�
        //------------------------------------------------------------------------------------------------------------------
        //ƴ��Ҫ�������ͼ����Դ��������ʹ��
        if (!G.ForbidAutoTexture) {
            this->PartName_TextureSlotReplace_Map.clear();
            for (const auto& pair : drawIBConfig.PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace(pair.second);
                this->PartName_TextureSlotReplace_Map[pair.first] = slotReplace;
            }
        }

        LOG.Info(L"Start to output ini file.");
        //��д��Constants���֣����Constantsû�У��ǾͲ�д����������Ҫ����һ�±���
        if (drawIBConfig.KeyNumber != 0) {
            M_IniSection ConstantsSection(IniSectionType::Constants);
            ConstantsSection.Append(L"[Constants]");
            ConstantsSection.Append(L"global $active" + std::to_wstring(GlobalActiveIndex));
            for (int i = 0; i < drawIBConfig.KeyNumber; i++) {
                //�м���key����������key
                std::string KeyStr = "global persist $swapkey" + std::to_string(i + GlobalKeyIndex_Constants) + " = 0";
                ConstantsSection.Append(MMTString::ToWideString(KeyStr));
            }
            ConstantsSection.NewLine();

            ZZZIniBuilder.AppendSection(ConstantsSection);


            M_IniSection PresentSection(IniSectionType::Present);
            PresentSection.Append(L"[Present]");
            PresentSection.Append(L"post $active"+std::to_wstring(GlobalActiveIndex) + L" = 0");
            PresentSection.NewLine();
            ZZZIniBuilder.AppendSection(PresentSection);

            
            for (const auto& TogglePair : drawIBConfig.ModelPrefixName_ToggleNameList_Map) {
                std::vector<std::string> ToggleNameList = TogglePair.second;
                if (ToggleNameList.size() >= 2) {
                    M_IniSection KeySection(IniSectionType::Key);
                    KeySection.Append(L"[KeySwap" + std::to_wstring(GlobalKeyIndex_Constants) + L"]");
                    KeySection.Append(L"condition = $active"+std::to_wstring(GlobalActiveIndex) + L" == 1");
                    KeySection.Append(L"key = " + DBMT_Constants::DefaultSwapCycleKeyList[GlobalKeyIndex_Constants]);
                    KeySection.Append(L"type = cycle");
                    std::wstring KeyCycleStr = L"";

                    for (int ToggleI = 0; ToggleI < ToggleNameList.size(); ToggleI++) {
                        if (ToggleI < ToggleNameList.size() - 1) {
                            KeyCycleStr = KeyCycleStr + std::to_wstring(ToggleI) + L",";
                        }
                        else {
                            KeyCycleStr = KeyCycleStr + std::to_wstring(ToggleI);
                        }
                    }

                    KeySection.Append(L"$swapkey" + std::to_wstring(GlobalKeyIndex_Constants) + L" = " + KeyCycleStr);
                    KeySection.NewLine();
                    ZZZIniBuilder.AppendSection(KeySection);

                    GlobalKeyIndex_Constants++;
                }

            }

        }
        //1.TextureOverrideVB���֣�ֻ��ʹ��GPU-PreSkinningʱ��ֱ���滻hash��Ӧ��λ
        if (d3d11GameType.GPUPreSkinning) {
            M_IniSection TextureOverrideVBSection(IniSectionType::TextureOverrideVB);
            LOG.Info(L"Start to output TextureOverrideVB section");
            TextureOverrideVBSection.Append(L"; " + drawIBConfig.DrawIB + L" -------------------------");
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

                    if (drawIBConfig.KeyNumber != 0) {
                        TextureOverrideVBSection.Append(L"$active"+std::to_wstring(GlobalActiveIndex) + L" = 1");
                    }
                }

                TextureOverrideVBSection.NewLine();
            }

            //2.VertexLimitRaise���֣�����ֻ��ʹ�õ�GPU-PreSkinning����ʱ����Ҫͻ�ƶ�����������
            //������drawNumber��ʵ�ֶ�̬����
            if (G.DynamicVertexLimitBreak) {
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_" + std::to_wstring(d3d11GameType.CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawIBConfig.DrawNumber) + L"_VertexLimitRaise]");
            }
            else {
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_VertexLimitRaise]");
            }
            TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(drawIBConfig.VertexLimitVB));
            TextureOverrideVBSection.NewLine();

            ZZZIniBuilder.AppendSection(TextureOverrideVBSection);
        }

        M_IniSection IBSkipSection(IniSectionType::IBSkip);
        IBSkipSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"IB]");
        IBSkipSection.Append(L"hash = " + drawIBConfig.DrawIB);
        IBSkipSection.Append(L"handling = skip");
        IBSkipSection.NewLine();
        ZZZIniBuilder.AppendSection(IBSkipSection);

        //2.TextureOverrideIB����
        M_IniSection TextureOverrideIB(IniSectionType::TextureOverrideIB);
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string IBFirstIndex = drawIBConfig.MatchFirstIndexList[i];
            std::string partName = drawIBConfig.PartNameList[i];
            std::wstring IBResourceName = L"Resource" + drawIBConfig.DrawIB + L"Index";

            TextureOverrideIB.Append(L"[TextureOverride" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L"]");
            TextureOverrideIB.Append(L"hash = " + drawIBConfig.DrawIB);
            TextureOverrideIB.Append(L"match_first_index = " + MMTString::ToWideString(IBFirstIndex));

            //��ͼ��λcheck���õ�������check����
            TextureOverrideIB.Append(L";Add slot check here to compatible with ZZMI if you manually add more ps slot replace for this IB's match_firt_index.");
            std::vector<std::wstring> slotReplaceStrList = this->PartName_TextureSlotReplace_Map[partName].Out_SlotReplaceStrList;
            if (!G.ForbidAutoTexture ) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    std::vector <std::wstring> splitList = MMTString::SplitString(slotReplaceStr, L"=");
                    TextureOverrideIB.Append(L"checktextureoverride = " + splitList[0]);
                }
            }

            //�滻IB��λ
            TextureOverrideIB.Append(L"ib = " + IBResourceName);

            //��ͼ��Դ�滻
            if (!G.ForbidAutoTexture && !G.UseHashTexture) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    TextureOverrideIB.Append(slotReplaceStr);
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
                            TextureOverrideIB.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                        }
                    }
                }
            }
            std::wstring ModelPrefixName = drawIB + L"-" + MMTString::ToWideString(partName);
            std::vector<std::string> ToggleNameList = drawIBConfig.ModelPrefixName_ToggleNameList_Map[MMTString::ToByteString(ModelPrefixName)];
            if (ToggleNameList.size() >= 2) {
                for (int CountI = 0; CountI < ToggleNameList.size(); CountI++) {
                    std::string ToggleName = ToggleNameList[CountI];

                    if (CountI == 0) {
                        TextureOverrideIB.Append(L"if $swapkey" + std::to_wstring(GlobalKeyIndex_Logic) + L" == " + std::to_wstring(CountI));
                    }
                    else {
                        TextureOverrideIB.Append(L"else if $swapkey" + std::to_wstring(GlobalKeyIndex_Logic) + L" == " + std::to_wstring(CountI));

                    }
                    
                    //Drawͼ��
                    std::wstring DrawIndexedStr = ToggleName_DrawIndexed_Map[ToggleName].ToDrawStr();
                    TextureOverrideIB.Append(L"  " + DrawIndexedStr);

                }
                TextureOverrideIB.Append(L"endif");

                TextureOverrideIB.NewLine();

                //ÿ����һ��ToggleNameList.size() > 2˵�����������ˣ�ͳ��ֵ��+1
                GlobalKeyIndex_Logic++;
            }
            else {
                std::wstring DrawIndexedStr = ToggleName_DrawIndexed_Map[ToggleNameList[0]].ToDrawStr();
                TextureOverrideIB.Append(DrawIndexedStr);
                TextureOverrideIB.NewLine();
            }

        }
        ZZZIniBuilder.AppendSection(TextureOverrideIB);


        //ResourceVB
        M_IniSection ResourceVBSection(IniSectionType::ResourceVB);
        for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
            ResourceVBSection.Append(L"[Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
            ResourceVBSection.Append(L"type = Buffer");
            
            if (categoryName == "Blend" && d3d11GameType.PatchBLENDWEIGHTS) {
                uint64_t finalBlendStride = d3d11GameType.CategoryStrideMap[categoryName] - d3d11GameType.ElementNameD3D11ElementMap["BLENDWEIGHTS"].ByteWidth;
                ResourceVBSection.Append(L"stride = " + std::to_wstring(finalBlendStride));
            }
            else {
                ResourceVBSection.Append(L"stride = " + std::to_wstring(d3d11GameType.CategoryStrideMap[categoryName]));
            }
            //vb�ļ����ļ��� 
            ResourceVBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf");
            ResourceVBSection.NewLine();
        }
        ZZZIniBuilder.AppendSection(ResourceVBSection);


        //ResourceIB
        M_IniSection ResourceIBSection(IniSectionType::ResourceIB);
        ResourceIBSection.Append(L"[Resource" + drawIBConfig.DrawIB + L"Index]");
        ResourceIBSection.Append(L"type = Buffer");
        ResourceIBSection.Append(L"format = DXGI_FORMAT_R32_UINT");
        ResourceIBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + L"Index.buf");
        ResourceIBSection.NewLine();
        ZZZIniBuilder.AppendSection(ResourceIBSection);

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
            ZZZIniBuilder.AppendSection(ResourceTextureSection);
        }


        
        LOG.NewLine();

        //�ƶ���λ��DDS�ļ���Mod�ļ�����
        BufferUtils::CopySlotDDSTextureFromOutputFolder(drawIBConfig);

        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

        GlobalActiveIndex++;
    }
    LOG.NewLine();

    if (!std::filesystem::exists(G.ModOutputFolder)) {
        LOG.Error(MMT_Tips::TIP_GenerateMod_NoAnyExportModelDetected);
    }
    else {
        std::vector<M_IniSection> HashTextureIniSectionList = BufferUtils::GenerateHashStyleTextureIni(this->DrawIB_ExtractConfig_Map);
        if (HashTextureIniSectionList.size() != 0) {
            SingleIniBuilder TextureIniBuilder;
            for (M_IniSection TextureSection : HashTextureIniSectionList) {
                TextureIniBuilder.AppendSection(TextureSection);
            }
            TextureIniBuilder.AppendSection(BufferUtils::GetCreditInfoIniSection());
            TextureIniBuilder.SaveToFile(G.ModOutputFolder + L"TextureReplace.ini");
        }
    }

    ZZZIniBuilder.AppendSection(BufferUtils::GetCreditInfoIniSection());

    ZZZIniBuilder.SaveToFile(G.ModOutputFolder + L"Config.ini");

    BufferUtils::WriteCreditREADME();

    LOG.NewLine();
}