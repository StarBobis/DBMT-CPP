#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "TextureSlotReplace.h"
#include "D3d11GameType.h"


class DrawIBConfig {
public:
	//2.通用项目
	std::wstring DrawIB;
	std::wstring GameType;

	//4.Split配置
	std::unordered_map <std::string, std::string> ColorMap;
	
	bool AverageNormalTANGENT = false;
	bool AverageNormalCOLOR = false;

	//翻转仅供测试使用，不提供给Mod作者作为可选项
	bool NormalReverseX = false;
	bool NormalReverseY = false;
	bool NormalReverseZ = false;
	bool TangentReverseX = false;
	bool TangentReverseY = false;
	bool TangentReverseZ = false;
	bool TangentReverseW = false;

	//这玩意Merge的时候保存到tmp.json，然后Split的时候从hash.json中提取
	std::unordered_map <std::string, std::string> CategoryHashMap;
	std::vector<std::string> MatchFirstIndexList;
	std::vector<std::string> PartNameList;
	std::vector<std::string> ImportModelList;
	std::vector<std::string> TmpElementList;
	std::string VertexLimitVB;
	std::string WorkGameType;

	std::unordered_map<std::string, std::vector<std::string>> PartName_TextureSlotReplace_Map;

	//新的贴图识别方式
	std::unordered_map<std::string, std::string> Hash_TextureFileName_Map;

	void SaveTmpJsonConfigs(std::wstring outputPath);
	void SaveTmpJsonConfigsV2(std::wstring outputPath);

	void SaveFormatJsonConfigs(std::string DXGIFORMAT);

	//下面这些是提取Mod时的属性
	std::wstring DrawIBOutputFolder;

	//下面这些是生成Mod时的属性
	D3D11GameType d3d11GameType;
	std::wstring ModOutputFolder;
	std::wstring BufferReadFolder;
	std::wstring BufferOutputFolder;
	std::wstring TextureOutputFolder;
	std::wstring CreditOutputFolder;
	std::wstring ModelFolder;
	uint64_t KeyNumber;
	
	std::unordered_map<std::string, std::vector<std::string>> ModelPrefixName_ToggleNameList_Map;//分支Mod支持
	uint64_t DrawNumber;
	std::unordered_map<std::wstring, std::vector<std::byte>> FinalVBCategoryDataMap;

	//下面分别是初始化提取和生成Mod所用到的文件夹和前置条件啥的
	bool InitializeGenerateMod(std::wstring Path_OutputFolder, D3D11GameType InputD3d11GameType);
	void CreateDrawIBOutputFolder(std::wstring Path_OutputFolder);
};