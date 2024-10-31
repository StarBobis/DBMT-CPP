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
    //��ȡ��ǰ���� YYYY_MM_DD
    //std::wstring timeStr = MMTString::GetFormattedDateTimeForFilename().substr(0, 10);
    
    //ʹ�ù̶�ֵ����������
    this->ModOutputFolder = Path_OutputFolder + L"GeneratedMod";

    // ָ�����Ŀ¼
    this->BufferReadFolder = Path_OutputFolder + this->DrawIB + L"\\";

    this->BufferOutputFolder = this->ModOutputFolder + L"\\Buffer\\";
    this->TextureOutputFolder = this->ModOutputFolder + L"\\Texture\\";
    this->ModelFolder = this->ModOutputFolder + L"\\Model\\";
    this->CreditOutputFolder = this->ModOutputFolder + L"\\Credit\\";

    this->d3d11GameType = InputD3d11GameType;

    std::wstring TmpJsonPath = this->BufferReadFolder + L"tmp.json";
    if (!std::filesystem::exists(TmpJsonPath)) {
        LOG.Error(L"1.���ɶ���ģ����Ҫ���Ƚ���������ȡģ�ͣ���������tmp.json�ļ��������ɶ���ģ�� 2.�������ֱ��������ȡ�Ļ����޷�����tmp.json�ģ�������ȡֻ��Ϊ�˻�ȡMod��ģ���ļ�������Ҫ��Mod�����޸Ĳ����뵽��Ϸ����Ҫ����������ȡԭʼģ��=>���ɶ���ģ�͵�Mod�������̡�");
    }

    //��ȡexport.json�е�����
    std::wstring ExportJsonPath = this->BufferReadFolder + L"export.json";
    uint64_t ExportModelNumber = 0;
    if (std::filesystem::exists(ExportJsonPath)) {
        //����������ʼ����֧Mod�����ࡣ
        nlohmann::json exportJsonData = MMTJson::ReadJsonFromFile(ExportJsonPath);
        //Ȼ�����ImportModelList����ȡÿ����λ�����Ĳ�λ����
        for (std::string ModelPrefixName: this->ImportModelList) {
            std::vector<std::string> ToggleNameList = exportJsonData[ModelPrefixName];
            this->ModelPrefixName_ToggleNameList_Map[ModelPrefixName] = ToggleNameList;
            ExportModelNumber += ToggleNameList.size();
        }
    }

    //��������ǰ׺�Զ�����Key����
    uint64_t TmpKeyNumber = 0;
    for (const auto& pair: this->ModelPrefixName_ToggleNameList_Map) {
        std::vector<std::string> ToggleNameList = pair.second;
        //ֻ��һ����ťʱ����Ҫ�л�����
        if (ToggleNameList.size() <= 1) {
            continue;
        }
        else {
            //���2������2�����ϣ�����Ҫһ��Key�����п����л�
            TmpKeyNumber++;
        }
    }
    LOG.Info(L"����Key��������" + std::to_wstring(TmpKeyNumber));
    this->KeyNumber = TmpKeyNumber;


    if (ExportModelNumber != 0) {
        //��ɾ����һ�����ɵ�����Buffer�ļ�
        //std::filesystem::remove_all(BufferOutputFolder);

        //�����´���Buffer�ļ���
        std::filesystem::create_directories(BufferOutputFolder);
        std::filesystem::create_directories(TextureOutputFolder);
        return true;
    }
    else {
        LOG.Warning(L"Detect didn't export vb file for DrawIB: " + this->DrawIB + L" , so skip this DrawIB's mod generate.");
        return false;
    }


}


//����outputFolder�������浽outputfolder�����DrawIBΪ���Ƶ��ļ�����
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
    // ��������ļ������ڣ�����Զ������µ��ļ���д�룬���Բ��õ��ġ�
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
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
    // ��������ļ������ڣ�����Զ������µ��ļ���д�룬���Բ��õ��ġ�
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
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

    //���浽�ļ�
    std::wstring configCompletePath = this->DrawIBOutputFolder + this->DrawIB + L"-" + this->GameType + L".json";
    std::ofstream file(configCompletePath);
    std::string content = FormatJson.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
    file << content << "\n";
    file.close();
}