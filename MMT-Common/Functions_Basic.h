#pragma once

#include <vector>
#include <unordered_map>

#include "D3d11GameType.h"
#include "D3D11Texture.h"
#include "TextureSlotReplace.h"
#include "MMTJsonUtils.h"
#include "DrawIBConfig.h"


//基类，方便管理和代码复用，存放案例方法子类可以有独特实现。
//工具类方法应该放到FunctionsBasicUtils里，此类仅表示抽象模型。
class Functions_Basic {
//仅允许子类访问的方法放到protected里，在这里存放公用方法。
protected:
	//贴图相关属性
	//Deprecated
	std::vector<TextureType> TextureTypeList;

	//Deprecated
	std::unordered_map<std::string, TextureSlotReplace> PartName_TextureSlotReplace_Map; //这个每个DrawIB用之前记得都清理一下，它只是起一个变量传递的作用。
	//新的自动贴图Map
	std::unordered_map<std::wstring, std::wstring> Total_Hash_TextureFileName_Map;

	//自动数据类型识别算法
	virtual std::wstring AutoGameType(std::wstring DrawIB);

	void InitializeGameTypeList(std::unordered_map<std::string, nlohmann::json> GameTypeName_GameTypeJson_Map);

	//下面这几个方法为初始化提取模型用到的东西
	void InitializeGameType(DrawIBConfig& In_DrawIBConfig);
//虚函数多态导出调用窗口，每个游戏都有自己的实现
public:
	//三合一包装数据类型，方便传递参数。
	D3D11GameTypeLv2 d3d11GameTypeLv2;

	std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map;

	//从配置文件中读取数据类型。
	virtual void InitializeGameTypeListFromConfig(std::wstring GameTypeConfigPath);
	//从代码中读取配置类型。
	virtual void InitializeGameTypeListFromPreset() {};
	//初始化贴图自动识别算法
	virtual void InitializeTextureTypeList() {};

	//最基础的提取模型和生成Mod 必选
	virtual void ExtractModel() { };
	//生成二创模型
	virtual void GenerateMod() { };
};

