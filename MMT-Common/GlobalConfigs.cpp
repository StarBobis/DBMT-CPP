#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"


//���������ȷ��ȫ��ΨһFAData
FrameAnalysisData GlobalConfigs::GetFrameAnalysisData() {
    //���Ϊ��˵��û��ʼ��,��Ȼֱ�ӷ����Ѿ���ʼ���õ�
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}

FrameAnalysisData GlobalConfigs::GetFrameAnalysisData(std::wstring DrawIB) {
    //���Ϊ��˵��û��ʼ��,��Ȼֱ�ӷ����Ѿ���ʼ���õ�
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder, DrawIB);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}


FrameAnalysisLog GlobalConfigs::GetFrameAnalysisLog() {
    if (this->FALog.WorkFolder == L"") {
        FrameAnalysisLog tmpFALog(this->WorkFolder);
        this->FALog = tmpFALog;
    }
    return this->FALog;
}


//���ṩ�������е�·��
GlobalConfigs::GlobalConfigs(std::wstring ApplicationLocation) {
    //��ʼ������·��
    this->Path_ApplicationRunningLocation = ApplicationLocation;
    LOG.Info(L"ApplicationLocation: " + ApplicationLocation);
    LOG.Info(L"Start to initialize global config:");

    //��ʼ������·��
    this->Path_MainJson = this->Path_ApplicationRunningLocation + L"\\Configs\\Main.json";

    //��ȡ��ǰ��������Ϸ����
    LOG.Info(L"Start to read from Main.json:");
    LOG.Info(L"Main.json Path:" + this->Path_MainJson);
    nlohmann::json MainJsonData = MMTJson::ReadJsonFromFile(this->Path_MainJson);
    this->CurrentGameName = MMTString::ToWideString(MainJsonData["GameName"]);
    LOG.Info(L"GameName:" + CurrentGameName);
    LOG.NewLine();

    //������Ϸ���ƣ������GamesĿ¼�µľ�����ϷĿ¼
    this->Path_Game_Folder = this->Path_ApplicationRunningLocation + L"\\Games\\" + CurrentGameName + L"\\";
    this->Path_Game_3DmigotoFolder = Path_Game_Folder + L"3Dmigoto\\";
    this->Path_Game_VSCheckJson = Path_Game_Folder + L"VSCheck.json";
    this->Path_Game_ConfigJson = Path_Game_Folder + L"Config.json";
    this->Path_Game_SettingJson = this->Path_ApplicationRunningLocation + L"\\Configs\\" + L"Setting.json";
    

    //�ļ����������
    this->Path_OutputFolder = this->Path_Game_3DmigotoFolder + L"Mods\\output\\";
    LOG.Info(L"OutputFolder:" + Path_OutputFolder);
    std::filesystem::create_directories(Path_OutputFolder);

    this->ModOutputFolder = this->Path_OutputFolder + L"GeneratedMod\\";
    this->Mod_BufferOutputFolder = this->ModOutputFolder + L"Buffer\\";
    this->Mod_TextureOutputFolder = this->ModOutputFolder + L"Texture\\";
    this->Mod_ModelOutputFolder = this->ModOutputFolder + L"Model\\";
    this->Mod_CreditOutputFolder = this->ModOutputFolder + L"Credit\\";
    LOG.Info(L"ModOutputFolder:" + this->ModOutputFolder);
    LOG.Info(L"Mod_BufferOutputFolder:" + this->Mod_BufferOutputFolder);
    LOG.Info(L"Mod_TextureOutputFolder:" + this->Mod_TextureOutputFolder);
    LOG.Info(L"Mod_ModelOutputFolder:" + this->Mod_ModelOutputFolder);
    LOG.Info(L"Mod_CreditOutputFolder:" + this->Mod_CreditOutputFolder);


    this->Path_LoaderFolder = this->Path_Game_3DmigotoFolder;
    LOG.Info(L"LoaderFolder:" + Path_LoaderFolder);
    
    std::wstring latestFrameAnalyseFolder = MMTFile::FindMaxPrefixedDirectory(Path_LoaderFolder, L"FrameAnalysis-");
    if (latestFrameAnalyseFolder == L"") {
        LOG.Warning(L"Can't find any FrameAnalysisFolder.");
    }
    this->Path_FrameAnalyseFolder = latestFrameAnalyseFolder;
    LOG.Info(L"FrameAnalyseFolder:" + this->Path_FrameAnalyseFolder);

    this->WorkFolder = this->Path_LoaderFolder + this->Path_FrameAnalyseFolder + L"\\";
    LOG.Info(L"WorkFolder:" + this->WorkFolder);

    this->DedupedFolder = this->WorkFolder + L"deduped\\";
    LOG.Info(L"DedupedFolder:" + this->DedupedFolder);
    LOG.NewLine();

    //��ȡ��ǰ�������������
    this->Path_RunInputJson = this->Path_ApplicationRunningLocation + L"\\Configs\\RunInput.json";
    nlohmann::json runInputJson = MMTJson::ReadJsonFromFile(this->Path_RunInputJson);
    std::string runCommand = runInputJson["RunCommand"];
    this->RunCommand = MMTString::ToWideString(runCommand);


    //��ȡȫ������ Configs\\Setting.json
    if (std::filesystem::exists(this->Path_Game_SettingJson)) {
        nlohmann::json configJsonData = MMTJson::ReadJsonFromFile(this->Path_Game_SettingJson);
        this->ShareSourceModel = (bool)configJsonData["ShareSourceModel"];
        LOG.Info(L"Share Source Model: " + std::to_wstring(this->ShareSourceModel));

        this->Author = MMTString::ToWideString((std::string)configJsonData["Author"]);
        LOG.Info(L"Author: " + this->Author);

        this->AuthorLink = MMTString::ToWideString((std::string)configJsonData["AuthorLink"]);
        LOG.Info(L"AuthorLink: " + this->AuthorLink);

        //��̬ͻ�ƶ�����������
        if (configJsonData.contains("DynamicVertexLimitBreak")) {
            this->DynamicVertexLimitBreak = (bool)configJsonData["DynamicVertexLimitBreak"];
            LOG.Info(L"DynamicVertexLimitBreak: " + std::to_wstring(this->DynamicVertexLimitBreak));
        }

        //MoveIBRelatedFiles
        if (configJsonData.contains("MoveIBRelatedFiles")) {
            this->MoveIBRelatedFiles = (bool)configJsonData["MoveIBRelatedFiles"];
            LOG.Info(L"MoveIBRelatedFiles: " + std::to_wstring(this->MoveIBRelatedFiles));
        }

        //DontSplitModelByMatchFirstIndex
        if (configJsonData.contains("DontSplitModelByMatchFirstIndex")) {
            this->DontSplitModelByMatchFirstIndex = (bool)configJsonData["DontSplitModelByMatchFirstIndex"];
            LOG.Info(L"DontSplitModelByMatchFirstIndex: " + std::to_wstring(this->DontSplitModelByMatchFirstIndex));
        }

        //ForbidAutoTexture
        if (configJsonData.contains("ForbidAutoTexture")) {
            this->ForbidAutoTexture = (bool)configJsonData["ForbidAutoTexture"];
            LOG.Info(L"ForbidAutoTexture: " + std::to_wstring(this->ForbidAutoTexture));
        }

        //UseHashTexture
        if (configJsonData.contains("UseHashTexture")) {
            this->UseHashTexture = (bool)configJsonData["UseHashTexture"];
            LOG.Info(L"UseHashTexture: " + std::to_wstring(this->UseHashTexture));
        }

        //ModelFileNameStyle
        if (configJsonData.contains("ModelFileNameStyle")) {
            this->ModelFileNameStyle = (int)configJsonData["ModelFileNameStyle"];
            LOG.Info(L"ModelFileNameStyle: " + std::to_wstring(this->ModelFileNameStyle));
        }
        LOG.NewLine();
    }

}


//�����ṩ�޲ι��죬������������ȫ�ֱ������ں������г�ʼ����
GlobalConfigs::GlobalConfigs() {
    
}




