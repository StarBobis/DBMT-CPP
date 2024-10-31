#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"


//用这个可以确保全局唯一FAData
FrameAnalysisData GlobalConfigs::GetFrameAnalysisData() {
    //如果为空说明没初始化,不然直接返回已经初始化好的
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}

FrameAnalysisData GlobalConfigs::GetFrameAnalysisData(std::wstring DrawIB) {
    //如果为空说明没初始化,不然直接返回已经初始化好的
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


//需提供程序运行的路径
GlobalConfigs::GlobalConfigs(std::wstring ApplicationLocation) {
    //初始化运行路径
    this->Path_ApplicationRunningLocation = ApplicationLocation;
    LOG.Info(L"ApplicationLocation: " + ApplicationLocation);
    LOG.Info(L"Start to initialize global config:");

    //初始化配置路径
    this->Path_MainJson = this->Path_ApplicationRunningLocation + L"\\Configs\\Main.json";

    //读取当前工作的游戏名称
    LOG.Info(L"Start to read from Main.json:");
    LOG.Info(L"Main.json Path:" + this->Path_MainJson);
    nlohmann::json MainJsonData = MMTJson::ReadJsonFromFile(this->Path_MainJson);
    this->CurrentGameName = MMTString::ToWideString(MainJsonData["GameName"]);
    LOG.Info(L"GameName:" + CurrentGameName);
    LOG.NewLine();

    //根据游戏名称，计算出Games目录下的具体游戏目录
    this->Path_Game_Folder = this->Path_ApplicationRunningLocation + L"\\Games\\" + CurrentGameName + L"\\";
    this->Path_Game_3DmigotoFolder = Path_Game_Folder + L"3Dmigoto\\";
    this->Path_Game_VSCheckJson = Path_Game_Folder + L"VSCheck.json";
    this->Path_Game_ConfigJson = Path_Game_Folder + L"Config.json";
    this->Path_Game_SettingJson = this->Path_ApplicationRunningLocation + L"\\Configs\\" + L"Setting.json";
    

    //文件夹相关设置
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

    //读取当前输入的运行命令
    this->Path_RunInputJson = this->Path_ApplicationRunningLocation + L"\\Configs\\RunInput.json";
    nlohmann::json runInputJson = MMTJson::ReadJsonFromFile(this->Path_RunInputJson);
    std::string runCommand = runInputJson["RunCommand"];
    this->RunCommand = MMTString::ToWideString(runCommand);


    //读取全局设置 Configs\\Setting.json
    if (std::filesystem::exists(this->Path_Game_SettingJson)) {
        nlohmann::json configJsonData = MMTJson::ReadJsonFromFile(this->Path_Game_SettingJson);
        this->ShareSourceModel = (bool)configJsonData["ShareSourceModel"];
        LOG.Info(L"Share Source Model: " + std::to_wstring(this->ShareSourceModel));

        this->Author = MMTString::ToWideString((std::string)configJsonData["Author"]);
        LOG.Info(L"Author: " + this->Author);

        this->AuthorLink = MMTString::ToWideString((std::string)configJsonData["AuthorLink"]);
        LOG.Info(L"AuthorLink: " + this->AuthorLink);

        //动态突破顶点数量限制
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


//必须提供无参构造，这样才能声明全局变量并在后续进行初始化。
GlobalConfigs::GlobalConfigs() {
    
}




