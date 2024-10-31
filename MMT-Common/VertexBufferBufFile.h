#pragma once
#include <iostream>
#include "D3d11GameType.h"



class VertexBufferBufFile {
public:
	//用于读取.ib .vb的vb文件并根据类型存储，方便后续分割位buf文件
	std::unordered_map<std::wstring, std::vector<std::byte>> CategoryVBDataMap;

	//用于读取FA文件夹中分成不同类别的BUF文件，组合成最终用于输出的数据
	std::vector<std::byte> FinalVB0Buf;

	VertexBufferBufFile();
	
	VertexBufferBufFile(std::wstring readVBBufFilePath,D3D11GameType d3d11GameType,std::vector<std::string> elementList);

	void SelfDivide(int MinNumber,int MaxNumber,int stride);
	void SaveToFile(std::wstring VB0OutputFilePath);

	
};

class VertexBuffer {
public:
	//顶点索引，数据
	std::unordered_map<uint64_t, std::vector<std::byte>> VertexIndex_ByteVector_Map;
	//整个文件所有数据
	std::vector<std::byte> ByteVector;

	VertexBuffer() {};

	VertexBuffer(std::wstring ReadFilePath, uint64_t VertexNumber);

	VertexBuffer(std::wstring ReadFilePath);
};