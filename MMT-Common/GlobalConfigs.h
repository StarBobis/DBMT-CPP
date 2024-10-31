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


//��������ʱ�����е�����Ӧ��������ȫ����ʼ�����
class GlobalConfigs {
protected:
	FrameAnalysisData FAData;
	FrameAnalysisLog FALog;
public:

	//��Ϸ����
	std::wstring CurrentGameName;
	//��ǰִ������
	std::wstring RunCommand;

	//�����õ�������·�����������ȷźã�����ֱ����
	std::wstring Path_MainJson;
	std::wstring Path_Game_Folder;
	std::wstring Path_Game_3DmigotoFolder;
	std::wstring Path_Game_VSCheckJson;
	std::wstring Path_Game_ConfigJson;
	std::wstring Path_Game_SettingJson;

	//��ȡ�������������ʹ��
	std::wstring Path_RunInputJson;

	//��ǰ����λ��
	std::wstring Path_ApplicationRunningLocation = L"";

	//�̶����ļ�������
	std::wstring Path_FrameAnalyseFolder;
	std::wstring Path_LoaderFolder;
	std::wstring Path_OutputFolder;

	//���Mod���ļ���
	std::wstring ModOutputFolder;
	std::wstring Mod_BufferOutputFolder;
	std::wstring Mod_TextureOutputFolder;
	std::wstring Mod_ModelOutputFolder;
	std::wstring Mod_CreditOutputFolder;


	std::wstring ShaderCheckConfigLocation;
	//�Ƿ���ҪVertexShaderCheck
	bool VertexShaderCheck = false;
	//��Ҫcheck��VertexShader��λ��һ��Ϊvb1,ib
	//std::vector<std::string> VertexShaderCheckList = { "vb1","ib" };
	std::vector<std::wstring> VertexShaderCheckList;
	std::wstring VertexShaderCheckListString;

	//��ѡ��Configs\Setting.json�е�����
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


	


	//����ʱ���ú���Ҫ�õ����ļ���
	std::wstring WorkFolder;
	std::wstring DedupedFolder;


	GlobalConfigs();
	GlobalConfigs(std::wstring);

	FrameAnalysisData GetFrameAnalysisData();
	FrameAnalysisData GetFrameAnalysisData(std::wstring DrawIB);
	FrameAnalysisLog GetFrameAnalysisLog();

};

//ȫ������
extern GlobalConfigs G;
