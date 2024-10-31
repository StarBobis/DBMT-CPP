#include "Functions_Basic.h"

#include "MMTJsonUtils.h"
#include "GlobalConfigs.h"


void Functions_Basic::InitializeGameTypeListFromConfig(std::wstring GameTypeConfigPath) {
	//ƴ��ɨ���Ŀ��Ŀ¼
	if (!std::filesystem::exists(GameTypeConfigPath)) {
		LOG.Error(L"��ǰ�����GameType�����б����ڡ�");
	}

	std::vector<std::wstring> GameTypeConfigJsonFileNameList = MMTFile::FindFileNameListWithCondition(GameTypeConfigPath, L"", L".json");

	std::unordered_map<std::string, nlohmann::json> GameTypeName_GameTypeJson_Map;

	for (std::wstring GameTypeJsonFileName : GameTypeConfigJsonFileNameList) {
		std::wstring GameTypeJsonPath = GameTypeConfigPath + GameTypeJsonFileName;
		nlohmann::json GameTypeJson = MMTJson::ReadJsonFromFile(GameTypeJsonPath);

		//�����GameType������ֱ�Ӷ�ȡ���ļ����ƣ���GUIͬ����
		std::string GameTypeName = MMTString::ToByteString(MMTString::GetFileNameWithOutSuffix(GameTypeJsonFileName));
		GameTypeName_GameTypeJson_Map[GameTypeName] = GameTypeJson;
	}

	this->InitializeGameTypeList(GameTypeName_GameTypeJson_Map);
}