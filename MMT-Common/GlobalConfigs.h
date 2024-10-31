#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <Windows.h>

#include <clocale>
#include <codecvt> 
#include <chrono>
#include <cmath>
#include <filesystem>
#include <map>
#include <unordered_map>

// algorithm
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string.hpp>
// date_time
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/local_time/local_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

// property_tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "D3d11GameType.h"

#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTFormatUtils.h"
#include "MMTJsonUtils.h"
#include "MMTLogUtils.h"

#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"

#include "D3D11Texture.h"
#include "DrawIBConfig.h"


//工具运行时，所有的配置应该在这里全部初始化完成
class GlobalConfigs {
protected:
	FrameAnalysisData FAData;
	FrameAnalysisLog FALog;
public:

	//游戏名称
	std::wstring CurrentGameName;
	//当前执行命令
	std::wstring RunCommand;

	//工具用到的所有路径都在这里先放好，后面直接用
	std::wstring Path_MainJson;
	std::wstring Path_Game_Folder;
	std::wstring Path_Game_3DmigotoFolder;
	std::wstring Path_Game_VSCheckJson;
	std::wstring Path_Game_ConfigJson;
	std::wstring Path_Game_SettingJson;

	//读取程序的输入命令使用
	std::wstring Path_RunInputJson;

	//当前运行位置
	std::wstring Path_ApplicationRunningLocation = L"";

	//固定的文件夹配置
	std::wstring Path_FrameAnalyseFolder;
	std::wstring Path_LoaderFolder;
	std::wstring Path_OutputFolder;

	//输出Mod的文件夹
	std::wstring ModOutputFolder;
	std::wstring Mod_BufferOutputFolder;
	std::wstring Mod_TextureOutputFolder;
	std::wstring Mod_ModelOutputFolder;
	std::wstring Mod_CreditOutputFolder;


	std::wstring ShaderCheckConfigLocation;
	//是否需要VertexShaderCheck
	bool VertexShaderCheck = false;
	//需要check的VertexShader槽位，一般为vb1,ib
	//std::vector<std::string> VertexShaderCheckList = { "vb1","ib" };
	std::vector<std::wstring> VertexShaderCheckList;
	std::wstring VertexShaderCheckListString;

	//首选项Configs\Setting.json中的设置
	bool ShareSourceModel = false;
	std::wstring Author;
	std::wstring AuthorLink;
	bool DynamicVertexLimitBreak = false;

	//MoveIBRelatedFiles
	bool MoveIBRelatedFiles = false;

	//DontSplitModelByMatchFirstIndex
	bool DontSplitModelByMatchFirstIndex = false;

	//ModelFileNameStyle
	int ModelFileNameStyle = 0;

	bool ForbidAutoTexture = false;
	
	bool UseHashTexture = false;


	


	//运行时设置好需要用到的文件夹
	std::wstring WorkFolder;
	std::wstring DedupedFolder;


	GlobalConfigs();
	GlobalConfigs(std::wstring);

	FrameAnalysisData GetFrameAnalysisData();
	FrameAnalysisData GetFrameAnalysisData(std::wstring DrawIB);
	FrameAnalysisLog GetFrameAnalysisLog();

};

//全局配置
extern GlobalConfigs G;
