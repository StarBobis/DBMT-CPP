#pragma once

#include "Functions_Basic.h"


class Functions_HSR :public Functions_Basic{
public:
	std::wstring AutoGameType(std::wstring DrawIB) override;

	void ExtractModel() override;

	void GenerateMod() override;

	void InitializeTextureTypeList() override;

};

