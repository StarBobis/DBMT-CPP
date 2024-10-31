#pragma once

#include <vector>
#include <unordered_map>

#include "D3d11GameType.h"
#include "D3D11Texture.h"
#include "TextureSlotReplace.h"
#include "MMTJsonUtils.h"
#include "DrawIBConfig.h"


//���࣬�������ʹ��븴�ã���Ű���������������ж���ʵ�֡�
//�����෽��Ӧ�÷ŵ�FunctionsBasicUtils��������ʾ����ģ�͡�
class Functions_Basic {
//������������ʵķ����ŵ�protected��������Ź��÷�����
protected:
	//��ͼ�������
	//Deprecated
	std::vector<TextureType> TextureTypeList;

	//Deprecated
	std::unordered_map<std::string, TextureSlotReplace> PartName_TextureSlotReplace_Map; //���ÿ��DrawIB��֮ǰ�ǵö�����һ�£���ֻ����һ���������ݵ����á�
	//�µ��Զ���ͼMap
	std::unordered_map<std::wstring, std::wstring> Total_Hash_TextureFileName_Map;

	//�Զ���������ʶ���㷨
	virtual std::wstring AutoGameType(std::wstring DrawIB);

	void InitializeGameTypeList(std::unordered_map<std::string, nlohmann::json> GameTypeName_GameTypeJson_Map);

	//�����⼸������Ϊ��ʼ����ȡģ���õ��Ķ���
	void InitializeGameType(DrawIBConfig& In_DrawIBConfig);
//�麯����̬�������ô��ڣ�ÿ����Ϸ�����Լ���ʵ��
public:
	//����һ��װ�������ͣ����㴫�ݲ�����
	D3D11GameTypeLv2 d3d11GameTypeLv2;

	std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map;

	//�������ļ��ж�ȡ�������͡�
	virtual void InitializeGameTypeListFromConfig(std::wstring GameTypeConfigPath);
	//�Ӵ����ж�ȡ�������͡�
	virtual void InitializeGameTypeListFromPreset() {};
	//��ʼ����ͼ�Զ�ʶ���㷨
	virtual void InitializeTextureTypeList() {};

	//���������ȡģ�ͺ�����Mod ��ѡ
	virtual void ExtractModel() { };
	//���ɶ���ģ��
	virtual void GenerateMod() { };
};

