#pragma once
#include <unordered_map>
#include <string>
#include "DrawIBConfig.h"


namespace FunctionsBasicUtils {
	std::unordered_map<std::wstring, DrawIBConfig> Read_DrawIB_ExtractConfig_Map();

	//移动DrawIB相关文件，用于快速测试新数据类型，避免Dump大量文件，解决账号无对应角色和皮肤的问题。
	void MoveDrawIBRelatedFiles_DEV(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);

	//读取d3dx.ini中的analyse_option然后输出日志
	void ShowAnalyseOptionsSetting_DEV();

	//移动所有贴图到output目录下对应文件夹
	void MoveAllUsedTexturesToOutputFolder(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map, bool ReverseExtract = false);
}