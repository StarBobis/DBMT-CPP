#pragma once

#include "Functions_Basic.h"

class Functions_WW11 :public Functions_Basic{
private:
	void ExtractModel_FromBuffer_WW11(std::wstring DrawIB, std::wstring GameType);
public:
	void ExtractModel() override;
	void GenerateMod() override;
	void InitializeTextureTypeList() override;
};
