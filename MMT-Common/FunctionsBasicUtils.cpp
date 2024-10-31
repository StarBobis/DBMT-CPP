#include "FunctionsBasicUtils.h"
#include "MMTJsonUtils.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"

#include "GlobalConfigs.h"

namespace FunctionsBasicUtils {

    std::unordered_map<std::wstring, DrawIBConfig> Read_DrawIB_ExtractConfig_Map() {
        std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map;
        //��ȡGames\\[��Ӧ��Ϸ����]\\Config.json����
        if (std::filesystem::exists(G.Path_Game_ConfigJson)) {
            //������Ϸ���ͣ���ȡconfigsĿ¼�¶�Ӧ�������ļ�
            nlohmann::json configJsonData = MMTJson::ReadJsonFromFile(G.Path_Game_ConfigJson);
            for (const auto& obj : configJsonData) {
                DrawIBConfig config;
                //�Ȱ�DrawIB��ȡ����
                config.DrawIB = MMTString::ToWideString(obj["DrawIB"]);
                LOG.Info(L"Start to read config for DrawIB:" + config.DrawIB);

                config.GameType = MMTString::ToWideString(obj["GameType"]);
                LOG.Info(L"GameType:" + config.GameType);

                if (obj.contains("AverageNormalCOLOR")) {
                    config.AverageNormalCOLOR = (bool)obj["AverageNormalCOLOR"];
                    LOG.Info(L"AverageNormalCOLOR:" + std::to_wstring(config.AverageNormalCOLOR));
                }

                if (obj.contains("AverageNormalTANGENT")) {
                    config.AverageNormalTANGENT = (bool)obj["AverageNormalTANGENT"];
                    LOG.Info(L"AverageNormalTANGENT:" + std::to_wstring(config.AverageNormalTANGENT));
                }

                //color map
                config.ColorMap = obj["Color"];
                LOG.Info(L"ColorMap:");
                for (const auto& pair : config.ColorMap) {
                    LOG.Info(L"Color: " + MMTString::ToWideString(pair.first) + L"  MMTString::value: " + MMTString::ToWideString(pair.second));
                }
                LOG.NewLine();

                //�������ж��Ƿ����tmpConfig,ֻ���ڴ��ڵ�����²Ż���ж�ȡ�������ֹ�ָ��
                std::wstring tmpConfigPath = G.Path_OutputFolder + config.DrawIB + L"\\tmp.json";
                LOG.Info(tmpConfigPath);
                if (std::filesystem::exists(tmpConfigPath)) {
                    nlohmann::json tmpJsonData = MMTJson::ReadJsonFromFile(tmpConfigPath);

                    //Read the config Merge generated.

                    LOG.Info(L"Start to read tmp config.");
                    config.VertexLimitVB = tmpJsonData["VertexLimitVB"];
                    LOG.Info(L"VertexLimitVB:" + MMTString::ToWideString(config.VertexLimitVB));

                    config.WorkGameType = tmpJsonData["WorkGameType"];
                    LOG.Info(L"WorkGameType:" + MMTString::ToWideString(config.WorkGameType));

                    config.TmpElementList = tmpJsonData["TmpElementList"];
                    LOG.Info(L"TmpElementList:");
                    for (const auto& pair : config.TmpElementList) {
                        LOG.Info(MMTString::ToWideString(pair));
                    }

                    config.CategoryHashMap = tmpJsonData["CategoryHash"];
                    LOG.Info(L"CategoryHash:");
                    for (const auto& pair : config.CategoryHashMap) {
                        LOG.Info(L"Category: " + MMTString::ToWideString(pair.first) + L"  Hash: " + MMTString::ToWideString(pair.second));
                    }
                    config.MatchFirstIndexList = tmpJsonData["MatchFirstIndex"];
                    LOG.Info(L"MatchFirstIndex:");
                    for (const auto& pair : config.MatchFirstIndexList) {
                        LOG.Info(MMTString::ToWideString(pair));
                    }
                    config.PartNameList = tmpJsonData["PartNameList"];
                    LOG.Info(L"PartNameList:");
                    for (const auto& pair : config.PartNameList) {
                        LOG.Info(MMTString::ToWideString(pair));
                    }

                    if (tmpJsonData.contains("ImportModelList")) {
                        config.ImportModelList = tmpJsonData["ImportModelList"];
                    }


                    if (tmpJsonData.contains("PartNameTextureResourceReplaceList")) {
                        config.PartName_TextureSlotReplace_Map = tmpJsonData["PartNameTextureResourceReplaceList"];
                    }

                    if (tmpJsonData.contains("Hash_TextureFileName_Map")) {
                        config.Hash_TextureFileName_Map = tmpJsonData["Hash_TextureFileName_Map"];
                    }

                    LOG.NewLine();
                }

                DrawIB_ExtractConfig_Map[config.DrawIB] = config;
            }
            LOG.Info(L"Read config for every DrawIB success.");
            LOG.NewLine();


        }
        else {
            LOG.Info(L"Can't find config file: " + G.Path_Game_ConfigJson);
        }
        return DrawIB_ExtractConfig_Map;
    }

    void MoveDrawIBRelatedFiles_DEV(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map) {

        if (G.MoveIBRelatedFiles) {
            LOG.Warning(L"���Թ��ܣ��ƶ�DrawIB����ļ���output�ļ����µ�DrawIBRelatedFiles\\FrameAnalysis-2028-08-28-666666�ļ����У�");
            FrameAnalysisData FAData = G.GetFrameAnalysisData();

            //�ƶ�����pointlist�ļ��Լ�DrawIB��ص�trianglelist�ļ���
            //�������õ������˷��������ļ��󣬾Ϳ��Խ���ֱ�Ӳ����ˡ�
            std::wstring OutputDrawIBRelatedFolder = G.Path_OutputFolder + L"DrawIBRelatedFiles\\FrameAnalysis-2028-08-28-666666\\";
            std::filesystem::create_directories(OutputDrawIBRelatedFolder);
            std::wstring DedupedFolder = OutputDrawIBRelatedFolder + L"deduped\\";
            std::filesystem::create_directories(DedupedFolder);

            std::map<std::wstring, uint64_t> PointlistIndexVertexCountMap = FAData.Get_PointlistIndex_VertexCount_Map(VertexCountType::BufFileRemoveNull);
            for (const auto& pair : PointlistIndexVertexCountMap) {
                std::vector<std::wstring> FileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(pair.first, L"");
                for (std::wstring FileName : FileNameList) {
                    std::filesystem::copy_file(G.WorkFolder + FileName, OutputDrawIBRelatedFolder + FileName, std::filesystem::copy_options::skip_existing);
                }
            }

            for (const auto& DrawIB_Pair : DrawIB_ExtractConfig_Map) {
                std::map<std::wstring, uint64_t> TrianglelistIndexVertexCountMap = FAData.Get_TrianglelistIndex_VertexCount_Map(VertexCountType::BufFileRemoveNull, DrawIB_Pair.first);
                for (const auto& pair : TrianglelistIndexVertexCountMap) {
                    std::vector<std::wstring> FileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(pair.first, L"");
                    for (std::wstring FileName : FileNameList) {
                        std::filesystem::copy_file(G.WorkFolder + FileName, OutputDrawIBRelatedFolder + FileName, std::filesystem::copy_options::skip_existing);
                    }
                }
            }

            //�ƶ�log.txt��ShaderUsage.txt
            if (std::filesystem::exists(G.WorkFolder + L"log.txt")) {
                std::filesystem::copy_file(G.WorkFolder + L"log.txt", OutputDrawIBRelatedFolder + L"log.txt", std::filesystem::copy_options::skip_existing);
            }

            if (std::filesystem::exists(G.WorkFolder + L"ShaderUsage.txt")) {
                std::filesystem::copy_file(G.WorkFolder + L"ShaderUsage.txt", OutputDrawIBRelatedFolder + L"ShaderUsage.txt", std::filesystem::copy_options::skip_existing);
            }

            LOG.NewLine();
        }
    }



    void ShowAnalyseOptionsSetting_DEV() {
        LOG.Info(L"Show [analyse_options] setting in d3dx.ini:");
        std::wstring D3dxiniPath = G.Path_LoaderFolder + L"d3dx.ini";
        std::vector<std::wstring> LineList = MMTFile::ReadAllLinesW(D3dxiniPath);
        for (std::wstring LineStr : LineList) {
            std::wstring LowerLineStr = boost::algorithm::to_lower_copy(LineStr);
            boost::algorithm::trim(LowerLineStr);
            if (LowerLineStr.starts_with(L"analyse_options")) {
                LOG.Info(L"Find:" + LowerLineStr);
                if (LowerLineStr.find(L"buf") == std::wstring::npos) {
                    LOG.Warning(L"��ǰanalyse_options��ȱ��buf�ļ���dump���ã�DBMT����������dump������buf�ļ��������analyse_options������dumpȻ�����ԡ�");
                }
            }
        }
        LOG.NewLine();
    }



    void MoveAllUsedTexturesToOutputFolder(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map, bool ReverseExtract) {
        LOG.Info(L"�ƶ������õ�����ͼ�ļ�:");
        FrameAnalysisData FAData = G.GetFrameAnalysisData();

        for (const auto& DrawIB_Pair : DrawIB_ExtractConfig_Map) {
            std::wstring DrawIB = DrawIB_Pair.first;
            LOG.Info(L"��ǰ����DrawIB: " + DrawIB);

            DrawIBConfig extractConfig = DrawIB_Pair.second;
            extractConfig.CreateDrawIBOutputFolder(G.Path_OutputFolder);

            std::wstring TrianglelistTexturesFolderPath = extractConfig.DrawIBOutputFolder + L"TrianglelistTextures\\";
            LOG.Info(L"TrianglelistTexturesFolderPath:" + TrianglelistTexturesFolderPath);
            std::wstring DedupedTexturesFolderPath = extractConfig.DrawIBOutputFolder + L"DedupedTextures\\";
            LOG.Info(L"DedupedTexturesFolderPath:" + DedupedTexturesFolderPath);

            std::filesystem::create_directories(DedupedTexturesFolderPath);
            std::filesystem::create_directories(TrianglelistTexturesFolderPath);

            //������ȡʱ��ʹ��������DrawIB������ȡ�������������ȡ������Ҫ��log�ļ��в�����ʵ��DrawIB
            std::wstring RealDrawIBFromLog = FAData.Get_RealDrawIB_FromLog(DrawIB);
            if (ReverseExtract) {
                LOG.Info(L"��ǰΪ������ȡģʽ��ʹ��RealDrawIBFromLog: " + RealDrawIBFromLog + L"���ԭ����DrawIB: " + DrawIB);
                DrawIB = RealDrawIBFromLog;
            }

            //ͨ��DrawIB������ص���ͼ�ļ�
            for (std::wstring index : FAData.ReadTrianglelistIndexList(DrawIB)) {
                LOG.Info(L"��ǰIndex: " + index);
                std::vector<std::wstring> pixelShaderTextureAllFileNameList = MMTFile::FindTextureFileList(G.WorkFolder, index + L"-ps-t");
                for (std::wstring psTextureFileName : pixelShaderTextureAllFileNameList) {
                    LOG.Info(L"Move: " + psTextureFileName);
                    std::filesystem::copy_file(G.WorkFolder + psTextureFileName, TrianglelistTexturesFolderPath + psTextureFileName, std::filesystem::copy_options::skip_existing);

                    std::wstring DedupedFileName = FAData.FindDedupedTextureName(G.WorkFolder, psTextureFileName);
                    if (DedupedFileName != L"") {
                        std::filesystem::copy_file(G.WorkFolder + psTextureFileName, DedupedTexturesFolderPath + DedupedFileName, std::filesystem::copy_options::skip_existing);
                    }
                }
            }
            LOG.NewLine();
        }

        LOG.Info(L"�����õ�����ͼ�ļ��ƶ���ϡ�");
        LOG.NewLine();
    }
}
