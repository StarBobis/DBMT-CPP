#pragma once

#include "Functions_Basic.h"


class Functions_GI : public Functions_Basic{

public:
	void ExtractModel() override;

	void GenerateMod() override;

	void InitializeTextureTypeList() override;

	std::wstring AutoGameType(std::wstring DrawIB);

};

