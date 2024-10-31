#include "Functions_GI.h"

#include "GlobalConfigs.h"
#include "D3d11GameType.h"
#include "VertexBufferBufFile.h"
#include "MMTConstants.h"

#include "BufferUtils.h"
#include "MigotoIniBuilder.h"

void Functions_GI::GenerateMod() {
    LOG.Info("Executing: GenerateMod_GI");

    SingleIniBuilder GIIniBuilder;

    //在Constants和Present，Key处声明时用的统计变量
    uint64_t GlobalKeyIndex_Constants = 0;
    //在ini中真实调用时用的统计变量
    uint64_t GlobalKeyIndex_Logic = 0;
    //每个Mod都会让active变量自增1
    uint64_t GlobalActiveIndex = 0;


    for (const auto& pair : this->DrawIB_ExtractConfig_Map) {

        std::wstring drawIB = pair.first;
        DrawIBConfig drawIBConfig = pair.second;
        drawIBConfig.GameType = MMTString::ToWideString(drawIBConfig.WorkGameType);
        D3D11GameType d3d11GameType = d3d11GameTypeLv2.GameTypeName_D3d11GameType_Map[drawIBConfig.WorkGameType];

        if (!drawIBConfig.InitializeGenerateMod(G.Path_OutputFolder, d3d11GameType)) {
            continue;
        }
        LOG.Info("D3d11GameType: " + d3d11GameType.GameType);


        //读取绘制的顶点数量
        drawIBConfig.DrawNumber = BufferUtilsLv2::GetSum_DrawNumber_FromVBFiles_Lv2(drawIBConfig);

        //读取VB文件中的数据
        drawIBConfig.FinalVBCategoryDataMap = BufferUtilsLv2::Read_FinalVBCategoryDataMap_Lv2(drawIBConfig);

        //数值处理
        if (drawIBConfig.AverageNormalTANGENT) {
            BufferUtils::Unity_TANGENT_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, d3d11GameType);
        }
        if (drawIBConfig.AverageNormalCOLOR) {
            BufferUtils::Unity_COLOR_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        }

        //Unity游戏需要翻转TANGENT的W分量
        BufferUtils::Unity_Reverse_NORMAL_TANGENT(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        BufferUtils::Unity_Reset_COLOR(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);

        BufferUtilsLv2::CombineIBToIndexBufFile_Lv2(drawIBConfig);
        std::unordered_map<std::string, M_DrawIndexed> ToggleName_DrawIndexed_Map = BufferUtilsLv2::Get_ToggleName_DrawIndexed_Map(drawIBConfig);


        BufferUtils::OutputCategoryBufferFiles(drawIBConfig);
        if (G.ShareSourceModel) {
            BufferUtilsLv2::ShareSourceModel_Lv2(drawIBConfig);
        }

        //------------------------------------------------------------------------------------------------------------------
        //拼接要输出的贴图的资源名供后续使用 
        if (!G.ForbidAutoTexture) {
            this->PartName_TextureSlotReplace_Map.clear();
            for (const auto& pair : drawIBConfig.PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace(pair.second);
                this->PartName_TextureSlotReplace_Map[pair.first] = slotReplace;
            }
        }
        

        LOG.Info(L"Start to combine ini file.");
        //先写出Constants部分，如果Constants没有，那就不写出，这里主要声明一下变量
        if (drawIBConfig.KeyNumber != 0) {
            M_IniSection ConstantsSection(IniSectionType::Constants);
            ConstantsSection.Append(L"[Constants]");
            ConstantsSection.Append(L"global $active" + std::to_wstring(GlobalActiveIndex));
            for (int i = 0; i < drawIBConfig.KeyNumber; i++) {
                //有几个key就声明几个key
                std::string KeyStr = "global persist $swapkey" + std::to_string(i + GlobalKeyIndex_Constants) + " = 0";
                ConstantsSection.Append(MMTString::ToWideString(KeyStr));
            }
            ConstantsSection.NewLine();

            GIIniBuilder.AppendSection(ConstantsSection);


            M_IniSection PresentSection(IniSectionType::Present);
            PresentSection.Append(L"[Present]");
            PresentSection.Append(L"post $active" + std::to_wstring(GlobalActiveIndex) + L" = 0");
            PresentSection.NewLine();
            GIIniBuilder.AppendSection(PresentSection);


            for (const auto& TogglePair : drawIBConfig.ModelPrefixName_ToggleNameList_Map) {
                std::vector<std::string> ToggleNameList = TogglePair.second;
                if (ToggleNameList.size() >= 2) {
                    M_IniSection KeySection(IniSectionType::Key);
                    KeySection.Append(L"[KeySwap" + std::to_wstring(GlobalKeyIndex_Constants) + L"]");
                    KeySection.Append(L"condition = $active" + std::to_wstring(GlobalActiveIndex) + L" == 1");
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
                    GIIniBuilder.AppendSection(KeySection);

                    GlobalKeyIndex_Constants++;
                }

            }

        }



        //1.TextureOverride VB部分，只有使用GPU-PreSkinning时是直接替换hash对应槽位
        if (d3d11GameType.GPUPreSkinning) {
            M_IniSection TextureOverrideVBSection(IniSectionType::TextureOverrideVB);
            LOG.Info(L"Start to output TextureOverrideVB section");
            for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
                TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(categoryHash));

                //遍历获取所有在当前分类hash下进行替换的分类，并添加对应的资源替换
                for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                    std::string originalCategoryName = pair.first;
                    std::string drawCategoryName = pair.second;
                    if (categoryName == drawCategoryName) {
                        std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                        TextureOverrideVBSection.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                    }
                }

                //draw一般都是在Blend槽位上进行的，所以我们这里要判断确定是Blend要替换的hash才能进行draw。
                if (categoryName == d3d11GameType.CategoryDrawCategoryMap["Blend"]) {
                    TextureOverrideVBSection.Append(L"handling = skip");
                    TextureOverrideVBSection.Append(L"draw = " + std::to_wstring(drawIBConfig.DrawNumber) + L", 0");

                    if (drawIBConfig.KeyNumber != 0) {
                        TextureOverrideVBSection.Append(L"$active" + std::to_wstring(GlobalActiveIndex) + L" = 1");
                    }
                }

                
                TextureOverrideVBSection.NewLine();
            }

            //2.VertexLimitRaise部分，这里只有使用到GPU-PreSkinning技术时才需要突破顶点数量限制
            if (G.DynamicVertexLimitBreak) {
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_" + std::to_wstring(d3d11GameType.CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawIBConfig.DrawNumber) + L"_VertexLimitRaise]");
            }
            else {
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_VertexLimitRaise]");
            }
            TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(drawIBConfig.VertexLimitVB));
            TextureOverrideVBSection.NewLine();

            GIIniBuilder.AppendSection(TextureOverrideVBSection);
        }

        //1.IB SKIP部分
        M_IniSection IBSkipSection(IniSectionType::IBSkip);
        IBSkipSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"IB]");
        IBSkipSection.Append(L"hash = " + drawIBConfig.DrawIB);
        IBSkipSection.Append(L"handling = skip");
        IBSkipSection.NewLine();
        GIIniBuilder.AppendSection(IBSkipSection);

        //2.IBOverride部分
        M_IniSection TextureOverrideIBSection(IniSectionType::TextureOverrideIB);

        uint64_t CurrentKeyIndex = 0;
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string IBFirstIndex = drawIBConfig.MatchFirstIndexList[i];
            std::string partName = drawIBConfig.PartNameList[i];

            std::wstring IBResourceName = L"Resource" + drawIBConfig.DrawIB + L"Index";

            TextureOverrideIBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + DBMT_Constants::GIMIPartNameAliasMap.at(MMTString::ToWideString(partName)) + L"]");
            TextureOverrideIBSection.Append(L"hash = " + drawIBConfig.DrawIB);
            TextureOverrideIBSection.Append(L"match_first_index = " + MMTString::ToWideString(IBFirstIndex));
          
            //替换IB槽位
            TextureOverrideIBSection.Append(L"ib = " + IBResourceName);

            //贴图资源替换
            std::vector<std::wstring> slotReplaceStrList = this->PartName_TextureSlotReplace_Map[partName].Out_SlotReplaceStrList;

            if (!G.ForbidAutoTexture && !G.UseHashTexture) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    TextureOverrideIBSection.Append(slotReplaceStr);
                }
            }
            

            //如果不使用GPU-Skinning即为Object类型，此时需要在ib下面替换对应槽位
            if (!d3d11GameType.GPUPreSkinning) {
                for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                    std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                    std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                    LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));

                    //遍历获取所有在当前分类hash下进行替换的分类，并添加对应的资源替换
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

            std::wstring ModelPrefixName = drawIB + L"-" + MMTString::ToWideString(partName);
            std::vector<std::string> ToggleNameList = drawIBConfig.ModelPrefixName_ToggleNameList_Map[MMTString::ToByteString(ModelPrefixName)];
            if (ToggleNameList.size() >= 2) {
                for (int CountI = 0; CountI < ToggleNameList.size(); CountI++) {
                    std::string ToggleName = ToggleNameList[CountI];

                    if (CountI == 0) {
                        TextureOverrideIBSection.Append(L"if $swapkey" + std::to_wstring(GlobalKeyIndex_Logic) + L" == " + std::to_wstring(CountI));
                    }
                    else {
                        TextureOverrideIBSection.Append(L"else if $swapkey" + std::to_wstring(GlobalKeyIndex_Logic) + L" == " + std::to_wstring(CountI));

                    }

                    //Draw图形
                    std::wstring DrawIndexedStr = ToggleName_DrawIndexed_Map[ToggleName].ToDrawStr();
                    TextureOverrideIBSection.Append(L"  " + DrawIndexedStr);

                }
                TextureOverrideIBSection.Append(L"endif");

                TextureOverrideIBSection.NewLine();

                //每触发一次ToggleNameList.size() > 2说明遇到按键了，统计值就+1
                GlobalKeyIndex_Logic++;
            }
            else {
                std::wstring DrawIndexedStr = ToggleName_DrawIndexed_Map[ToggleNameList[0]].ToDrawStr();
                TextureOverrideIBSection.Append(DrawIndexedStr);
                TextureOverrideIBSection.NewLine();
            }
            
        }
        GIIniBuilder.AppendSection(TextureOverrideIBSection);

        //5.写出VBResource部分
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
            //vb文件的文件名
            ResourceVBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf");
            ResourceVBSection.NewLine();
        }
        GIIniBuilder.AppendSection(ResourceVBSection);


        //写出IBResource， 也就是Index.buf
        M_IniSection ResourceIBSection(IniSectionType::ResourceIB);
        ResourceIBSection.Append(L"[Resource" + drawIBConfig.DrawIB + L"Index]");
        ResourceIBSection.Append(L"type = Buffer");
        ResourceIBSection.Append(L"format = DXGI_FORMAT_R32_UINT");
        ResourceIBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + L"Index.buf");
        ResourceIBSection.NewLine();
        GIIniBuilder.AppendSection(ResourceIBSection);

        //7.写出贴图resource部分
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
            GIIniBuilder.AppendSection(ResourceTextureSection);
        }


        //写完关闭文件
        LOG.NewLine();

        //移动槽位的DDS文件到Mod文件夹中
        BufferUtils::CopySlotDDSTextureFromOutputFolder(drawIBConfig);

        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

        GlobalActiveIndex++;
    }

    //TODO 需要先把这个改成一个方法，能够对每个DrawIB进行调用才行。
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

    GIIniBuilder.AppendSection(BufferUtils::GetCreditInfoIniSection());
    GIIniBuilder.SaveToFile(G.ModOutputFolder + L"Config.ini");

    BufferUtils::WriteCreditREADME();

    LOG.NewLine();
}