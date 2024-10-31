#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

//字符串常用boost方法
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>

namespace MMTString {
	//字符串转换
	std::wstring ToWideString(std::string input);
	std::string	ToByteString(std::wstring input);
	std::wstring ToLowerCase(std::wstring str);
	std::string ToLowerCase(std::string str);
	std::wstring ToUpperCase(std::wstring str);
	std::string ToUpperCase(std::string str);

	//获取当前工作路径
	std::wstring GetCurrentWorkingDirectoryPath();

	//根据当前目录路径，获取父级目录路径
	std::wstring GetParentFolderPathFromFolderPath(std::wstring FolderPath);

	// 根据文件完整路径获取文件所在目录路径,结尾不会自动补充/
	std::wstring GetFolderPathFromFilePath(std::wstring filePath);



	//生成UUID
	std::wstring GenerateUUIDW();

	//生成日期字符串
	std::wstring GetFormattedDateTimeForFilename();

	//纯C++实现分割字符串，因为Boost库无法正常以L"=="作为分隔符
	//原始字符串，用于分割的字符串
	std::vector<std::wstring> SplitString(std::wstring originalStr, std::wstring delimiter);
	std::vector <std::wstring> SplitStringOnlyMatchFirst(std::wstring originalStr, std::wstring delimiter);

	//通过文件路径获取文件名
	std::wstring GetFileNameFromFilePath(std::wstring filePath);


	//查找最后一个.的位置并返回.前面的所有部分，如果没找到最后一个.则返回原本的路径。
	std::wstring GetFileNameWithOutSuffix(std::wstring filePath);

	//移除文件完整路径结尾的后缀名并改为新的后缀名后返回。
	std::wstring GetNewFileNamePathWithNewSuffix(std::wstring ReadFilePath, std::wstring NewSuffix);



	LPSTR WCHAR_TO_LPSTR(WCHAR wStr[]);

	//获取文件名中包含的属性
	std::wstring GetFileHashFromFileName(std::wstring input);
	std::wstring GetPSHashFromFileName(const std::wstring& input);
	std::wstring GetVSHashFromFileName(const std::wstring& input);
	std::wstring GetPixelSlotFromTextureFileName(std::wstring TextureFileName);

	//移除字符串中的内容
	std::wstring RemoveSquareBrackets(std::wstring inputStr);
	std::wstring Remove$Prefix(std::wstring VariableName);
}
