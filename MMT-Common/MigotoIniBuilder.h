#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "MMTConstants.h"
#include "MigotoFormat.h"


class SingleIniBuilder {
private:
	std::vector<std::wstring> LineList;
	
	void AppendSectionLine(IniSectionType InputIniSectionType);//Deprecated

	std::vector<M_IniSection> IniSectionList;

public:
	void AppendSection(M_IniSection InputIniSection);
	
	void SaveToFile(std::wstring FilePath);//Deprecated
};


class ModIniBuilder {
	std::unordered_map<std::wstring, SingleIniBuilder> DrawIB_SingleIniBuilder_Map;

	void SaveToFolder(std::wstring Path_OutputFolder, bool CombineToOneMod);
};