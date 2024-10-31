#pragma once
#include <unordered_map>
#include <string>
#include "DrawIBConfig.h"


namespace FunctionsBasicUtils {
	std::unordered_map<std::wstring, DrawIBConfig> Read_DrawIB_ExtractConfig_Map();

	//�ƶ�DrawIB����ļ������ڿ��ٲ������������ͣ�����Dump�����ļ�������˺��޶�Ӧ��ɫ��Ƥ�������⡣
	void MoveDrawIBRelatedFiles_DEV(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map);

	//��ȡd3dx.ini�е�analyse_optionȻ�������־
	void ShowAnalyseOptionsSetting_DEV();

	//�ƶ�������ͼ��outputĿ¼�¶�Ӧ�ļ���
	void MoveAllUsedTexturesToOutputFolder(std::unordered_map<std::wstring, DrawIBConfig> DrawIB_ExtractConfig_Map, bool ReverseExtract = false);
}