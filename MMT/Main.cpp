#include <io.h>
#include "GlobalConfigs.h"

//CPU-PreSkinning
#include "Functions_Unity_CPU_PreSkinning.h"

//UE4 CS
#include "Functions_WW.h"
#include "Functions_ToF.h"

//UE4 VS
#include "Functions_WW1.1.h"
#include "Functions_KBY.h"
#include "Functions_SnB.h"

//Unity VS
#include "Functions_HI3.h"
#include "Functions_GI.h"
#include "Functions_HSR.h"
#include "Functions_ZZZ.h"

//Unity CS

#include "MMTTimeUtils.h"

#include "FunctionsBasicUtils.h"

//初始化easylogpp
INITIALIZE_EASYLOGGINGPP
//初始化全局配置
GlobalConfigs G;
//初始化日志
MMTLogger LOG;


std::int32_t wmain(std::int32_t argc, wchar_t* argv[])
{
    try {

        //设置使用中文locale
        setlocale(LC_ALL, "Chinese-simplified");

        std::wstring fullPath = argv[0];
        std::wstring applicationLocation = MMTString::GetFolderPathFromFilePath(fullPath);

        //因为我们需要放到Plugins目录中
        applicationLocation = MMTString::GetParentFolderPathFromFolderPath(applicationLocation);

        //首先初始化日志配置，非常重要
        std::string logFileName = "Logs\\" + MMTTime::GetCurrentTimeString() + ".log";
        el::Configurations logConfigurations;
        logConfigurations.setToDefault();
        logConfigurations.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
        logConfigurations.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
        logConfigurations.set(el::Level::Global, el::ConfigurationType::Filename, logFileName);
        el::Loggers::reconfigureAllLoggers(logConfigurations);

        LOG.Info(L"Running : " + fullPath);
        LOG.NewLine();

        //初始化日志类
        LOG = MMTLogger(applicationLocation);
        //初始化全局配置
        G = GlobalConfigs(applicationLocation);

        //初始化各游戏处理程序
        LOG.Info(L"欢迎使用DBMT(DirectX Buffer Mod Tool)，本程序由NicoMico开发，可在粉丝群获取");
        LOG.Info(L"当前执行命令：" + G.RunCommand);
        LOG.NewLine();

        //根据游戏类型决定具体使用哪个子类进行处理
        Functions_Basic* basic_functions = new Functions_Basic();

        if (G.CurrentGameName == L"GI") {
            basic_functions = new Functions_GI();
        }
        else if (G.CurrentGameName == L"HI3") {
            basic_functions = new Functions_HI3();
        }
        else if (G.CurrentGameName == L"HSR") {
            basic_functions = new Functions_HSR();
        }
        else if (G.CurrentGameName == L"ZZZ") {
            basic_functions = new Functions_ZZZ();
        }
        else if (G.CurrentGameName == L"WW1.1") {
            basic_functions = new Functions_WW11();
        }
        else if (G.CurrentGameName == L"SnB") {
            basic_functions = new Functions_SnB();
        }
        else if (G.CurrentGameName == L"KBY") {
            basic_functions = new Functions_KBY();
        }
        else if (G.CurrentGameName == L"ToF") {
            basic_functions = new Functions_ToF();
        }
        else if (G.CurrentGameName == L"WW") {
            basic_functions = new Functions_WW();
        }
        //CPU-PreSkinning的游戏
        else if (G.CurrentGameName == L"Unity-CPU-PreSkinning") {
            basic_functions = new Functions_Unity_CPU_PreSkinning();
        }

        //初始化贴图识别算法列表
        basic_functions->InitializeTextureTypeList();

        //初始化该游戏数据类型
        std::wstring GameTypeConfigPath = G.Path_ApplicationRunningLocation + L"\\Configs\\ExtractTypes\\" + G.CurrentGameName + L"\\";
        //if (G.CurrentGameName == L"Unity-CPU-PreSkinning") {
        //    //Unity-CPU-PreSkinning
        //    GameTypeConfigPath = G.Path_ApplicationRunningLocation + L"\\Configs\\ExtractTypes\\Unity-CPU-PreSkinning\\";
        //}
        basic_functions->InitializeGameTypeListFromConfig(GameTypeConfigPath);

        //额外添加逆向提取的数据类型路径
        std::wstring ReverseGameTypeExtraConfigPath = G.Path_ApplicationRunningLocation + L"\\Configs\\ReverseTypes\\";

        //读取DrawIB配置
        basic_functions->DrawIB_ExtractConfig_Map = FunctionsBasicUtils::Read_DrawIB_ExtractConfig_Map();


        LOG.NewLine();
#ifdef _DEBUG 
        //注意！禁止使用DEBUG模式进行测试和编译！
        LOG.NewLine();
#else
        //正常提取模型
        if (G.RunCommand == L"merge") {
            //如果运行为Merge，则确保存在至少一个FrameAnalysis文件夹
            if (G.Path_FrameAnalyseFolder == L"") {
                LOG.Error("Can't find any FrameAnalysis folder in your 3Dmigoto folder,please try press F8 to dump a new one with Hunting open.");
            }
            else {
                //显示analyse_options设置
                FunctionsBasicUtils::ShowAnalyseOptionsSetting_DEV();
                //移动DrawIB相关文件
                FunctionsBasicUtils::MoveDrawIBRelatedFiles_DEV(basic_functions->DrawIB_ExtractConfig_Map);

                basic_functions->ExtractModel();
            }
        }

        //生成Mod
        else if (G.RunCommand == L"split") {

            if (G.CurrentGameName == L"WWMI") {
                LOG.Error(L"WWMI仅用于逆向提取和脚本式一键逆向，如需使用WW功能请切换到WW1.1");
            }
            else {
                basic_functions->GenerateMod();
            }
        }
        else {
            LOG.Error(L"未知的命令，请检查您的MMT版本");
        }

        //If we success executed,we will run to here.
        LOG.Success();

        //clean object created by "new" 
        delete basic_functions;
#endif
        return 0;

    }
    catch (const std::exception& e) {
        LOG.Error(e.what());
    }
}
