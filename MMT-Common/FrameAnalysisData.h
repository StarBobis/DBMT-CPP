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


	//缓存机制，防止多次重复任务
	std::unordered_map<std::wstring, std::map<std::wstring, uint64_t>> DrawIB_TrianglelistIndex_VertexCount_Map;
	std::map<std::wstring, uint64_t> PointlistIndex_VertexCount_Map;

public:
	std::wstring WorkFolder = L"";
	
	std::vector<std::wstring> FrameAnalysisFileNameList;
	void ReadFrameAnalysisFileList(std::wstring WorkFolder);


	//这个旧的设计不能删，有部分逻辑还是用的上的
	std::vector<std::wstring> TrianglelistIndexList;
	std::vector<std::wstring> PointlistIndexList;
	std::vector<std::wstring> ReadTrianglelistIndexList(std::wstring drawIB);
	std::vector<std::wstring> ReadPointlistIndexList();

	//顶点数量从vb0中读取，依赖于.txt文件存在
	std::map<std::wstring, uint64_t> Get_TrianglelistIndex_VertexCount_Map(VertexCountType InputVertexCountType,std::wstring DrawIB);

	//顶点数量从vb0中读取，依赖于.txt文件存在
	std::map<std::wstring, uint64_t> Get_PointlistIndex_VertexCount_Map(VertexCountType InputVertexCountType);


	uint64_t GetTrianglelistMaxVertexCount(VertexCountType InputVertexCountType, std::wstring DrawIB);
	std::wstring GetTrianglelistExtractIndex(VertexCountType InputVertexCountType, std::wstring DrawIB, D3D11GameType d3d11GameType, bool checkTexcoordSlotExists);
	std::wstring GetPointlistExtractIndex(VertexCountType InputVertexCountType, uint64_t TrianglelistMaxVertexCount);

	//展示读取的Pointlist和Trianglelist对应的Index_VertexCount_Map
	void Show_Pointlist_Index_VertexCount_Map();
	void Show_Trianglelist_Index_VertexCount_Map(std::wstring DrawIB);


	FrameAnalysisData();
	FrameAnalysisData(std::wstring WorkFolder);
	FrameAnalysisData(std::wstring WorkFolder,std::wstring DrawIB);


	std::wstring Get_RealDrawIB_FromLog(std::wstring DrawIB); //通过DrawIB获取原始DrawIB

	std::vector<std::wstring> FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr);

	std::wstring FindDedupedTextureName(std::wstring WorkFolder, std::wstring TextureFileName);
	std::wstring GetIBMatchFirstIndexByIndex(std::wstring WorkFolder, std::wstring Index);

	std::unordered_map<std::wstring, IndexBufferBufFile> Get_MatchFirstIndex_IBBufFile_Map_FromLog(std::wstring DrawIB);
	IndexBufferBufFile GetIBBufFileByIndex(std::wstring Index);

	std::vector<std::wstring> ReadRealTrianglelistIndexListFromLog(std::wstring OriginalDrawIB);

	std::unordered_map<std::wstring, uint64_t> Get_Index_CS_CB0_VertexCount_Map_FromCSBufferFiles();
	std::wstring GetComputeIndexFromCSCB0_ByVertexCount(uint64_t VertexCount);

	std::map<uint64_t, std::wstring> Get_MatchFirstIndex_IBFileName_Map(std::wstring DrawIB);

	//TODO 要注意的是使用IB中独立顶点数的做法来统计可能会在数据量过大时增加统计时间，每个游戏是否准确也不尽相同。
	uint64_t Get_VertexCount_ByAddIBFileUniqueCount(std::wstring DrawIB);


	std::unordered_map<std::string, std::vector<std::byte>> ReadCategoryBufferFile(std::wstring BufferReadPath, std::string CategoryName, D3D11GameType d3d11GameType);


};