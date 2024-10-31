#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

class IndexBufferTxtFile {
	public:
		//��Ҫ�ֶ�
		std::wstring FileName;
		std::wstring Index;
		std::wstring Hash;
		std::wstring FirstIndex = L"0"; //Ĭ��Ϊ0�������ȡ�����������0����
		std::wstring IndexCount;
		std::wstring Topology;
		std::wstring Format;
		std::wstring ByteOffset;

		uint64_t MaxNumber = 0;
		uint64_t MinNumber = 99999999;
		uint64_t UniqueVertexCount = 0;
		uint64_t IndexNumberCount = 0;

		//�����б�
		std::vector<uint64_t> NumberList;
		
		std::unordered_map<int, std::vector<std::byte>> IBTxtToBufMap;

		IndexBufferTxtFile();

		IndexBufferTxtFile(std::wstring fileName, bool readIBData);

		void SaveToFile_UINT32(std::wstring FileWritePath, uint64_t Offset);

};