#pragma once
#include <string>
//使用const std::wstring在namespace里声明常量这样就可以全局使用一份常量了


enum class VertexCountType {
	TxtFileShow,
	TxtFileCountLine,
	BufFileTotal,
	BufFileRemoveNull
};


enum class IniSectionType {
	Present,
	Constants,
	Key,
	TextureOverrideIB,
	TextureOverrideVB,
	TextureOverrideTexture,
	IBSkip,
	ResourceVB,
	ResourceIB,
	ResourceTexture,
	CreditInfo
};


namespace DBMT_Constants {

	//目前Unity游戏还没见过超过8个的
	const std::vector<std::wstring> DefaultSwapCycleKeyList = { L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j" };


	//这里注意，unordered_map使用const初始化后，只能用.at()访问其元素，不可使用[]访问
	const std::unordered_map<std::wstring, std::wstring> GIMIPartNameAliasMap = {
		{L"1",L"Head" },
		{L"2",L"Body"},
		{L"3",L"Dress"},
		{L"4",L"Extra"},
		{L"5",L"Extra1"},
		{L"6",L"Extra2"},
		{L"7",L"Extra3"},
		{L"8",L"Extra4"},
		{L"9",L"Extra5"},
		{L"10",L"Extra6"},
		{L"11",L"Extra7"},
		{L"12",L"Extra8"},
		{L"13",L"Extra9"},
		{L"14",L"Extra10"},
		{L"15",L"Extra11"},
		{L"16",L"Extra12"},
		{L"17",L"Extra13"}
	};

}



namespace MMT_Tips {
	const std::wstring TIP_GenerateMod_NoAnyExportModelDetected = LR"(
在您的从Blender导出到的DrawIB的Hash文件夹中，未找到任何.ib .vb .fmt格式的3Dmigoto模型文件
可能的解决方案如下：
1,请检查您是否正确地使用MMT插件进行了导出？
2,请检查您导出的文件格式是否为[数字.后缀]格式例如1.ib 1.vb 1.fmt？
3,请检查您是否导出了从游戏中Dump的全部部位？如果您是在修改其它人的Mod模型可能他们的模型部位与当前游戏提取出的部位不一致，
请补充导出对应部位并在生成Mod后取消Mod的ini中对应部位的TextureOverrideIB中的drawindexed = auto或设置ib = null
4,如反复检查确定已经导出成功并且格式全部正确，请联系NicoMico获取技术支持。)";

	const std::wstring TIP_AutoGameType_NoGameTypeDetected = LR"(
基于数据类型的自动类型识别无法识别出当前IB的数据类型
可能的解决方案如下：
1,请联系NicoMico测试并添加此数据类型支持，您需要将本次运行读取的最新FrameAnalysis文件夹以及提取用的IB的Hash值一起发送给NicoMico来让他测试并在新版本中添加支持。
2,检查您是否在更新工具时未更新Config目录下的ExtractTypes文件夹，导致游戏版本更新后数据类型和MMT的最新数据类型不匹配，导致无法识别。
3,如果您在之前的版本可以正常提取，而当前版本无法提取，请联系NicoMico修复此BUG。
)";

	const std::wstring TIP_CantFindExtractIndex = LR"(
无法找到TrianglelistExtract Index! 
可能的解决方案如下：
1.如果游戏是GI,请检查是否开启了角色动态高精度，如果开启了请关闭 。
2.请检查你在Dump之前是否按小键盘+号重置了Hunting界面 ，如果未重置可能会导致此问题。
3.如果重置了Hunting界面还不行，请删除Mod重启游戏再尝试一次，因为打了Mod的Buffer在内存中大小会被扩充导致无法识别，
即使F6关掉Mod也无法改变内存中Buffer大小，除非在关了Mod的情况下切换场景或重启游戏让此模型的Buffer在内存中重新加载，
所以一般出现这种情况关掉Mod重启游戏即可解决。
)";
}


namespace DXGI_FORMAT {
	const std::string R8G8B8A8_UNORM = "R8G8B8A8_UNORM";
	const std::string R32G32B32A32_FLOAT = "R32G32B32A32_FLOAT";
	const std::string DXGI_FORMAT_R16_UINT = "DXGI_FORMAT_R16_UINT";
	const std::string DXGI_FORMAT_R32_UINT = "DXGI_FORMAT_R32_UINT";

}



