#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

class IndexBufferTxtFile {
	public:
		//主要字段
		std::wstring FileName;
		std::wstring Index;
		std::wstring Hash;
		std::wstring FirstIndex = L"0"; //默认为0，如果读取不到就用这个0好了
		std::wstring IndexCount;
		std::wstring Topology;
		std::wstring Format;
		std::wstring ByteOffset;

		uint64_t MaxNumber = 0;
		uint64_t MinNumber = 99999999;
		uint64_t UniqueVertexCount = 0;
		uint64_t IndexNumberCount = 0;

		//顶点列表
		std::vector<uint64_t> NumberList;
		
		std::unordered_map<int, std::vector<std::byte>> IBTxtToBufMap;

		IndexBufferTxtFile();

		IndexBufferTxtFile(std::wstring fileName, bool readIBData);

		void SaveToFile_UINT32(std::wstring FileWritePath, uint64_t Offset);

};