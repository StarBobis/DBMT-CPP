#pragma once
#include <string>
//ʹ��const std::wstring��namespace���������������Ϳ���ȫ��ʹ��һ�ݳ�����


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

	//ĿǰUnity��Ϸ��û��������8����
	const std::vector<std::wstring> DefaultSwapCycleKeyList = { L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j", L"n",L"m",L"k",L"l",L"u",L"i",L"o",L"p",L"b",L"j" };


	//����ע�⣬unordered_mapʹ��const��ʼ����ֻ����.at()������Ԫ�أ�����ʹ��[]����
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
�����Ĵ�Blender��������DrawIB��Hash�ļ����У�δ�ҵ��κ�.ib .vb .fmt��ʽ��3Dmigotoģ���ļ�
���ܵĽ���������£�
1,�������Ƿ���ȷ��ʹ��MMT��������˵�����
2,�������������ļ���ʽ�Ƿ�Ϊ[����.��׺]��ʽ����1.ib 1.vb 1.fmt��
3,�������Ƿ񵼳��˴���Ϸ��Dump��ȫ����λ������������޸������˵�Modģ�Ϳ������ǵ�ģ�Ͳ�λ�뵱ǰ��Ϸ��ȡ���Ĳ�λ��һ�£�
�벹�䵼����Ӧ��λ��������Mod��ȡ��Mod��ini�ж�Ӧ��λ��TextureOverrideIB�е�drawindexed = auto������ib = null
4,�練�����ȷ���Ѿ������ɹ����Ҹ�ʽȫ����ȷ������ϵNicoMico��ȡ����֧�֡�)";

	const std::wstring TIP_AutoGameType_NoGameTypeDetected = LR"(
�����������͵��Զ�����ʶ���޷�ʶ�����ǰIB����������
���ܵĽ���������£�
1,����ϵNicoMico���Բ���Ӵ���������֧�֣�����Ҫ���������ж�ȡ������FrameAnalysis�ļ����Լ���ȡ�õ�IB��Hashֵһ���͸�NicoMico���������Բ����°汾�����֧�֡�
2,������Ƿ��ڸ��¹���ʱδ����ConfigĿ¼�µ�ExtractTypes�ļ��У�������Ϸ�汾���º��������ͺ�MMT�������������Ͳ�ƥ�䣬�����޷�ʶ��
3,�������֮ǰ�İ汾����������ȡ������ǰ�汾�޷���ȡ������ϵNicoMico�޸���BUG��
)";

	const std::wstring TIP_CantFindExtractIndex = LR"(
�޷��ҵ�TrianglelistExtract Index! 
���ܵĽ���������£�
1.�����Ϸ��GI,�����Ƿ����˽�ɫ��̬�߾��ȣ������������ر� ��
2.��������Dump֮ǰ�Ƿ�С����+��������Hunting���� �����δ���ÿ��ܻᵼ�´����⡣
3.���������Hunting���滹���У���ɾ��Mod������Ϸ�ٳ���һ�Σ���Ϊ����Mod��Buffer���ڴ��д�С�ᱻ���䵼���޷�ʶ��
��ʹF6�ص�ModҲ�޷��ı��ڴ���Buffer��С�������ڹ���Mod��������л�������������Ϸ�ô�ģ�͵�Buffer���ڴ������¼��أ�
����һ�������������ص�Mod������Ϸ���ɽ����
)";
}


namespace DXGI_FORMAT {
	const std::string R8G8B8A8_UNORM = "R8G8B8A8_UNORM";
	const std::string R32G32B32A32_FLOAT = "R32G32B32A32_FLOAT";
	const std::string DXGI_FORMAT_R16_UINT = "DXGI_FORMAT_R16_UINT";
	const std::string DXGI_FORMAT_R32_UINT = "DXGI_FORMAT_R32_UINT";

}



