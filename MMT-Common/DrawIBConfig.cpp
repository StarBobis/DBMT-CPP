#include "DrawIBConfig.h"

#include "MMTLogUtils.h"
#include "MMTJsonUtils.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"

#include <set>
#include <algorithm>


void DrawIBConfig::CreateDrawIBOutputFolder(std::wstring Path_OutputFolder) {
    this->DrawIBOutputFolder = Path_OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(this->DrawIBOutputFolder);
}


bool DrawIBConfig::InitializeGenerateMod(std::wstring Path_OutputFolder, D3D11GameType InputD3d11GameType) {
    //获取当前日期 YYYY_MM_DD
    //std::wstring timeStr = MMTString::GetFormattedDateTimeForFilename().substr(0, 10);
    
    //使用固定值而不是日期
    this->ModOutputFolder = Path_OutputFolder + L"GeneratedMod";

    // 指定输出目录
    this->BufferReadFolder = Path_OutputFolder + this->DrawIB + L"\\";

    this->BufferOutputFolder = this->ModOutputFolder + L"\\Buffer\\";
    this->TextureOutputFolder = this->ModOutputFolder + L"\\Texture\\";
    this->ModelFolder = this->ModOutputFolder + L"\\Model\\";
    this->CreditOutputFolder = this->ModOutputFolder + L"\\Credit\\";

    this->d3d11GameType = InputD3d11GameType;

    std::wstring TmpJsonPath = this->BufferReadFolder + L"tmp.json";
    if (!std::filesystem::exists(TmpJsonPath)) {
        LOG.Error(L"1.生成二创模型需要你先进行正向提取模型，才能生成tmp.json文件用于生成二创模型 2.如果你是直接逆向提取的话是无法生成tmp.json的，逆向提取只是为了获取Mod的模型文件，如需要对Mod进行修改并导入到游戏，需要走完整的提取原始模型=>生成二创模型的Mod制作流程。");
    }

    //读取export.json中的内容
    std::wstring ExportJsonPath = this->BufferReadFolder + L"export.json";
    uint64_t ExportModelNumber = 0;
    if (std::filesystem::exists(ExportJsonPath)) {
        //如果存在则初始化分支Mod抽象类。
        nlohmann::json exportJsonData = MMTJson::ReadJsonFromFile(ExportJsonPath);
        //然后遍历ImportModelList，获取每个部位导出的部位名称
        for (std::string ModelPrefixName: this->ImportModelList) {
            std::vector<std::string> ToggleNameList = exportJsonData[ModelPrefixName];
            this->ModelPrefixName_ToggleNameList_Map[ModelPrefixName] = ToggleNameList;
            ExportModelNumber += ToggleNameList.size();
        }
    }

    //根据名称前缀自动生成Key数量
    uint64_t TmpKeyNumber = 0;
    for (const auto& pair: this->ModelPrefixName_ToggleNameList_Map) {
        std::vector<std::string> ToggleNameList = pair.second;
        //只有一个按钮时不需要切换按键
        if (ToggleNameList.size() <= 1) {
            continue;
        }
        else {
            //如果2个或者2个以上，则需要一个Key来进行控制切换
            TmpKeyNumber++;
        }
    }
    LOG.Info(L"所需Key的数量：" + std::to_wstring(TmpKeyNumber));
    this->KeyNumber = TmpKeyNumber;


    if (ExportModelNumber != 0) {
        //先删除上一次生成的所有Buffer文件
        //std::filesystem::remove_all(BufferOutputFolder);

        //再重新创建Buffer文件夹
        std::filesystem::create_directories(BufferOutputFolder);
        std::filesystem::create_directories(TextureOutputFolder);
        return true;
    }
    else {
        LOG.Warning(L"Detect didn't export vb file for DrawIB: " + this->DrawIB + L" , so skip this DrawIB's mod generate.");
        return false;
    }


}


//接收outputFolder，并保存到outputfolder下面的DrawIB为名称的文件夹里
void DrawIBConfig::SaveTmpJsonConfigs(std::wstring outputPath) {
    LOG.Info(L"Start to save tmp config");

    //write to json file.
    nlohmann::json tmpJsonData;
    tmpJsonData["VertexLimitVB"] = VertexLimitVB;
    tmpJsonData["TmpElementList"] = TmpElementList;
    tmpJsonData["CategoryHash"] = CategoryHashMap;
    tmpJsonData["MatchFirstIndex"] = MatchFirstIndexList;
    tmpJsonData["PartNameList"] = PartNameList;
    tmpJsonData["ImportModelList"] = ImportModelList;
    tmpJsonData["WorkGameType"] = WorkGameType;

    tmpJsonData["PartNameTextureResourceReplaceList"] = this->PartName_TextureSlotReplace_Map;

    std::wstring configCompletePath = outputPath + L"tmp.json";
    LOG.Info(L"configCompletePath: " + configCompletePath);
    // save json config into file.
    // 这里如果文件不存在，则会自动创建新的文件来写入，所以不用担心。
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // 使用缩进格式保存到文件，缩进为4个空格
    file << content << "\n";
    file.close();
    LOG.Info(L"Json config has saved into file.");

    LOG.NewLine();
}


void DrawIBConfig::SaveTmpJsonConfigsV2(std::wstring outputPath) {
    LOG.Info(L"Start to save tmp config");

    //write to json file.
    nlohmann::json tmpJsonData;
    tmpJsonData["VertexLimitVB"] = VertexLimitVB;
    tmpJsonData["TmpElementList"] = TmpElementList;
    tmpJsonData["CategoryHash"] = CategoryHashMap;
    tmpJsonData["MatchFirstIndex"] = MatchFirstIndexList;
    tmpJsonData["PartNameList"] = PartNameList;
    tmpJsonData["WorkGameType"] = WorkGameType;
    tmpJsonData["ImportModelList"] = ImportModelList;
    tmpJsonData["Hash_TextureFileName_Map"] = this->Hash_TextureFileName_Map;

    std::wstring configCompletePath = outputPath + L"tmp.json";
    LOG.Info(L"configCompletePath: " + configCompletePath);
    // save json config into file.
    // 这里如果文件不存在，则会自动创建新的文件来写入，所以不用担心。
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // 使用缩进格式保存到文件，缩进为4个空格
    file << content << "\n";
    file.close();
    LOG.Info(L"Json config has saved into file.");

    LOG.NewLine();

}


void DrawIBConfig::SaveFormatJsonConfigs(std::string DXGIFORMAT) {

    nlohmann::json FormatJson;
    FormatJson["BufferList"] = nlohmann::json::array();

    for (std::string ElementName: d3d11GameType.OrderedFullElementList) {
        nlohmann::json BufferJson;
        BufferJson["FileName"] = MMTString::ToByteString(this->DrawIB) + "-" + ElementName + ".buf";
        BufferJson["Format"] = d3d11GameType.ElementNameD3D11ElementMap[ElementName].Format;
        BufferJson["ElementName"] = ElementName;
        FormatJson["BufferList"].push_back(BufferJson);
    }

    for (std::string PartName : this->PartNameList) {
        nlohmann::json IndexBufferJson;
        IndexBufferJson["Format"] = DXGIFORMAT;
        IndexBufferJson["FileName"] = MMTString::ToByteString(this->DrawIB) + "-Index" + PartName + ".buf";
        IndexBufferJson["ElementName"] = "Index";
        FormatJson["BufferList"].push_back(IndexBufferJson);
    }

    //保存到文件
    std::wstring configCompletePath = this->DrawIBOutputFolder + this->DrawIB + L"-" + this->GameType + L".json";
    std::ofstream file(configCompletePath);
    std::string content = FormatJson.dump(4); // 使用缩进格式保存到文件，缩进为4个空格
    file << content << "\n";
    file.close();
}