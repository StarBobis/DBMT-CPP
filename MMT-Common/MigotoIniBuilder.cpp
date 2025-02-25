#include "MigotoIniBuilder.h"

#include "MMTFileUtils.h"
#include "MigotoFormat.h"


void SingleIniBuilder::AppendSectionLine(IniSectionType InputIniSectionType) {
	for (M_IniSection iniSection : this->IniSectionList) {
		if (iniSection.SectionType == InputIniSectionType) {
			for (std::wstring iniLine : iniSection.SectionLineList) {
				LineList.push_back(iniLine);
			}
			LineList.push_back(L";----------------------------------------------------------\n");
		}
	}
}


void SingleIniBuilder::AppendSection(M_IniSection InputIniSection) {
	this->IniSectionList.push_back(InputIniSection);
}


void SingleIniBuilder::SaveToFile(std::wstring FilePath) {
	//这里并不是简单地输出就行了，而是根据Section判断来进行排列后再输出
	// 所以外部不允许调用AppendSectionLine方法，外部只接收新增Section，且Section必须指定类型。
	this->AppendSectionLine(IniSectionType::Constants);
	this->AppendSectionLine(IniSectionType::Present);
	this->AppendSectionLine(IniSectionType::Key);

	this->AppendSectionLine(IniSectionType::IBSkip);

	this->AppendSectionLine(IniSectionType::TextureOverrideIB);
	this->AppendSectionLine(IniSectionType::TextureOverrideVB);

	this->AppendSectionLine(IniSectionType::TextureOverrideTexture);

	this->AppendSectionLine(IniSectionType::ResourceIB);
	this->AppendSectionLine(IniSectionType::ResourceVB);
	this->AppendSectionLine(IniSectionType::ResourceTexture);

	this->AppendSectionLine(IniSectionType::CreditInfo);

	std::wofstream outputIniFile(FilePath);
	for (std::wstring Line: this->LineList) {
		outputIniFile << Line << L"\n";
	}
	outputIniFile.close();
}


void ModIniBuilder::SaveToFolder(std::wstring Path_OutputFolder, bool CombineToOneMod) {


}



