#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:
	std::wstring Index;
	std::wstring MatchFirstIndex;
	uint64_t ReadDrawNumber = 0;

	//最小的顶点数值
	uint64_t MinNumber = 0;
	//最大的顶点数值
	uint64_t MaxNumber = 0;
	//总共有几个索引
	uint64_t NumberCount = 0;
	//总共用到的独立顶点数量
	uint64_t UniqueVertexCount = 0;

	//顶点列表
	//多个IB文件组合在一起时，建议使用NumberList来进行组合而不是二进制格式
	std::vector<uint64_t> NumberList;

	IndexBufferBufFile();

	//IB文件完整路径，不区分大小写的格式例如DXGI_FORMAT_R32_UINT
	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SelfDivide(int FirstIndex,int IndexCount);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint64_t Offset);
	void SaveToFile_UINT16(std::wstring FileWritePath, uint16_t Offset);

	void SaveToFile_MinSize(std::wstring FileWritePath, uint64_t Offset);
	std::string GetSelfMinFormat();

	IndexBufferBufFile(std::vector<uint64_t> NumberList);
	void AddOffset(uint64_t Offset);
	void UpdateAttributes();

};