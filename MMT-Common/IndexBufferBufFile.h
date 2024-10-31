#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:
	std::wstring Index;
	std::wstring MatchFirstIndex;
	uint64_t ReadDrawNumber = 0;

	//��С�Ķ�����ֵ
	uint64_t MinNumber = 0;
	//���Ķ�����ֵ
	uint64_t MaxNumber = 0;
	//�ܹ��м�������
	uint64_t NumberCount = 0;
	//�ܹ��õ��Ķ�����������
	uint64_t UniqueVertexCount = 0;

	//�����б�
	//���IB�ļ������һ��ʱ������ʹ��NumberList��������϶����Ƕ����Ƹ�ʽ
	std::vector<uint64_t> NumberList;

	IndexBufferBufFile();

	//IB�ļ�����·���������ִ�Сд�ĸ�ʽ����DXGI_FORMAT_R32_UINT
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