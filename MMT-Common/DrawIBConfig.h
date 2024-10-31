#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "TextureSlotReplace.h"
#include "D3d11GameType.h"


class DrawIBConfig {
public:
	//2.ͨ����Ŀ
	std::wstring DrawIB;
	std::wstring GameType;

	//4.Split����
	std::unordered_map <std::string, std::string> ColorMap;
	
	bool AverageNormalTANGENT = false;
	bool AverageNormalCOLOR = false;

	//��ת��������ʹ�ã����ṩ��Mod������Ϊ��ѡ��
	bool NormalReverseX = false;
	bool NormalReverseY = false;
	bool NormalReverseZ = false;
	bool TangentReverseX = false;
	bool TangentReverseY = false;
	bool TangentReverseZ = false;
	bool TangentReverseW = false;

	//������Merge��ʱ�򱣴浽tmp.json��Ȼ��Split��ʱ���hash.json����ȡ
	std::unordered_map <std::string, std::string> CategoryHashMap;
	std::vector<std::string> MatchFirstIndexList;
	std::vector<std::string> PartNameList;
	std::vector<std::string> ImportModelList;
	std::vector<std::string> TmpElementList;
	std::string VertexLimitVB;
	std::string WorkGameType;

	std::unordered_map<std::string, std::vector<std::string>> PartName_TextureSlotReplace_Map;

	//�µ���ͼʶ��ʽ
	std::unordered_map<std::string, std::string> Hash_TextureFileName_Map;

	void SaveTmpJsonConfigs(std::wstring outputPath);
	void SaveTmpJsonConfigsV2(std::wstring outputPath);

	void SaveFormatJsonConfigs(std::string DXGIFORMAT);

	//������Щ����ȡModʱ������
	std::wstring DrawIBOutputFolder;

	//������Щ������Modʱ������
	D3D11GameType d3d11GameType;
	std::wstring ModOutputFolder;
	std::wstring BufferReadFolder;
	std::wstring BufferOutputFolder;
	std::wstring TextureOutputFolder;
	std::wstring CreditOutputFolder;
	std::wstring ModelFolder;
	uint64_t KeyNumber;
	
	std::unordered_map<std::string, std::vector<std::string>> ModelPrefixName_ToggleNameList_Map;//��֧Mod֧��
	uint64_t DrawNumber;
	std::unordered_map<std::wstring, std::vector<std::byte>> FinalVBCategoryDataMap;

	//����ֱ��ǳ�ʼ����ȡ������Mod���õ����ļ��к�ǰ������ɶ��
	bool InitializeGenerateMod(std::wstring Path_OutputFolder, D3D11GameType InputD3d11GameType);
	void CreateDrawIBOutputFolder(std::wstring Path_OutputFolder);
};