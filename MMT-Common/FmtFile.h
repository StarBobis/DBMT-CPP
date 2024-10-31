#pragma once
//#include "GlobalConfigs.h"
#include "D3d11GameType.h"

class FmtFile {
public:
	std::wstring Topology = L"";
	std::vector<D3D11Element> d3d11ElementList;
	std::unordered_map<std::string, D3D11Element> ElementName_D3D11Element_Map;

	//���������������������Ҫ�ֶ�ƴ��һ��FMT�ļ������ʱ�������õĶ���
	std::wstring Format = L""; //Ĭ��ʹ��R32_UINT������������������ֶ����á�
	std::wstring Prefix = L""; //ָ��ib��vb�ļ��Ĺ�ͬǰ׺������ʱһ��Ҫ�õ���
	D3D11GameType d3d11GameType;
	std::vector<std::string> ElementNameList;
	int Stride = 0;


	FmtFile();
	FmtFile(std::wstring readFmtPath);
	void OutputFmtFile(std::wstring OutputFmtPath);

	bool IsD3d11ElementListMatch(std::vector<std::string> TargetElementList);
};


