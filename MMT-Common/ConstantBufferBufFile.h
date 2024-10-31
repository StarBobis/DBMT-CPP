#pragma once
#include <cstdint>
#include <map>
#include <iostream>

class ConstantBufferValue {
public:
	uint64_t X;
	uint64_t Y;
	uint64_t Z;
	uint64_t W;
};


class ConstantBufferBufFile {
public:
	std::map<int, ConstantBufferValue> lineCBValueMap;

	ConstantBufferBufFile();
	ConstantBufferBufFile(std::wstring CBFilePath);
};