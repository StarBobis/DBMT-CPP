#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include "IndexBufferBufFile.h"
#include "MMTConstants.h"
#include "D3d11GameType.h"



class FrameAnalysisData {
protected:
	std::unordered_map<std::wstring, uint64_t> Index_CS_CB0_VertexCount_Map;


	//������ƣ���ֹ����ظ�����
	std::unordered_map<std::wstring, std::map<std::wstring, uint64_t>> DrawIB_TrianglelistIndex_VertexCount_Map;
	std::map<std::wstring, uint64_t> PointlistIndex_VertexCount_Map;

public:
	std::wstring WorkFolder = L"";
	
	std::vector<std::wstring> FrameAnalysisFileNameList;
	void ReadFrameAnalysisFileList(std::wstring WorkFolder);


	//����ɵ���Ʋ���ɾ���в����߼������õ��ϵ�
	std::vector<std::wstring> TrianglelistIndexList;
	std::vector<std::wstring> PointlistIndexList;
	std::vector<std::wstring> ReadTrianglelistIndexList(std::wstring drawIB);
	std::vector<std::wstring> ReadPointlistIndexList();

	//����������vb0�ж�ȡ��������.txt�ļ�����
	std::map<std::wstring, uint64_t> Get_TrianglelistIndex_VertexCount_Map(VertexCountType InputVertexCountType,std::wstring DrawIB);

	//����������vb0�ж�ȡ��������.txt�ļ�����
	std::map<std::wstring, uint64_t> Get_PointlistIndex_VertexCount_Map(VertexCountType InputVertexCountType);


	uint64_t GetTrianglelistMaxVertexCount(VertexCountType InputVertexCountType, std::wstring DrawIB);
	std::wstring GetTrianglelistExtractIndex(VertexCountType InputVertexCountType, std::wstring DrawIB, D3D11GameType d3d11GameType, bool checkTexcoordSlotExists);
	std::wstring GetPointlistExtractIndex(VertexCountType InputVertexCountType, uint64_t TrianglelistMaxVertexCount);

	//չʾ��ȡ��Pointlist��Trianglelist��Ӧ��Index_VertexCount_Map
	void Show_Pointlist_Index_VertexCount_Map();
	void Show_Trianglelist_Index_VertexCount_Map(std::wstring DrawIB);


	FrameAnalysisData();
	FrameAnalysisData(std::wstring WorkFolder);
	FrameAnalysisData(std::wstring WorkFolder,std::wstring DrawIB);


	std::wstring Get_RealDrawIB_FromLog(std::wstring DrawIB); //ͨ��DrawIB��ȡԭʼDrawIB

	std::vector<std::wstring> FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr);

	std::wstring FindDedupedTextureName(std::wstring WorkFolder, std::wstring TextureFileName);
	std::wstring GetIBMatchFirstIndexByIndex(std::wstring WorkFolder, std::wstring Index);

	std::unordered_map<std::wstring, IndexBufferBufFile> Get_MatchFirstIndex_IBBufFile_Map_FromLog(std::wstring DrawIB);
	IndexBufferBufFile GetIBBufFileByIndex(std::wstring Index);

	std::vector<std::wstring> ReadRealTrianglelistIndexListFromLog(std::wstring OriginalDrawIB);

	std::unordered_map<std::wstring, uint64_t> Get_Index_CS_CB0_VertexCount_Map_FromCSBufferFiles();
	std::wstring GetComputeIndexFromCSCB0_ByVertexCount(uint64_t VertexCount);

	std::map<uint64_t, std::wstring> Get_MatchFirstIndex_IBFileName_Map(std::wstring DrawIB);

	//TODO Ҫע�����ʹ��IB�ж�����������������ͳ�ƿ��ܻ�������������ʱ����ͳ��ʱ�䣬ÿ����Ϸ�Ƿ�׼ȷҲ������ͬ��
	uint64_t Get_VertexCount_ByAddIBFileUniqueCount(std::wstring DrawIB);


	std::unordered_map<std::string, std::vector<std::byte>> ReadCategoryBufferFile(std::wstring BufferReadPath, std::string CategoryName, D3D11GameType d3d11GameType);


};