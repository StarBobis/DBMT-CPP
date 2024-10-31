#pragma once
#pragma once
#include <vector>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>
#include "IndexBufferBufFile.h"
#include "MMTConstants.h"

#include <unordered_map>
#include <map>
//-----------------------------------------------------------------------------------------------------------------------------------
// 这里存放基础数据类型，指的是从ini里进行分析而提取的第一层抽象信息，方便后续所有的分析过程
// 用于解析3Dmigoto的基本数据类型
//-----------------------------------------------------------------------------------------------------------------------------------
class IniLineObject {
public:
	std::wstring LeftStr;
	std::wstring LeftStrTrim;
	std::wstring RightStr;
	std::wstring RightStrTrim;
	bool valid = false;

	IniLineObject();
	IniLineObject(std::wstring readLine);
	//下面这个留着指定分隔符，上面那个用来判断变量相关的时候
	IniLineObject(std::wstring readLine, std::wstring delimiter);
};


class MigotoAttribute {
public:
	//这里存放3Dmigoto的ini的通用属性，比如NameSpace等等。
	std::wstring NameSpace = L"";
};


//通用类，逆向、正向、Mod合并和分析都要用到。
class M_IniSection : public MigotoAttribute {
public:
	IniSectionType SectionType;
	std::wstring SectionName;
	std::vector<std::wstring> SectionLineList;

	M_IniSection() {};
	
	M_IniSection(IniSectionType InputSectionType) {
		this->SectionType = InputSectionType;
	};

	void Append(std::wstring InputLine) {
		this->SectionLineList.push_back(InputLine);
	}

	void NewLine() {
		this->SectionLineList.push_back(L"");
	}
};


class M_ExpressionValue {
public:
	//抽象语法执行树
	/*
	算法如下，依次扫描并添加每个元素到列表中
	再遍历这个列表来处理整个表达式
	因为我们暂时只需要考虑变量名和+ - 两个符号
	遇到变量名则获取变量名，遇到符号则执行逻辑，需要在执行时存储上一个命令
	需要一个解析和执行方法
	*/
	std::wstring ExpressionOriginalLine;
	std::vector<std::wstring> ExpressionList;
	void ParseExpression();

	//此变量在构造函数执行完毕后被赋值，默认为false
	bool isPureValue = false;
	M_ExpressionValue() {};
	M_ExpressionValue(std::wstring ExpressionStringLine);
};


//同时用于constants和present
class M_Variable : public MigotoAttribute {
public:
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal expression  在使用的时候区别是表达式类型还是普通类型，如果是表达式类型记得去计算结果。
	M_ExpressionValue ExpressionValue;

	M_Variable();
	//无初始值的变量
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType);
	//带有初始值的变量
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType);

	//表达式类型的变量
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, M_ExpressionValue In_m_ExpressionValue, std::wstring InType);
};


//Condition代表一个条件，一个Condition中可以有多个对比表达式
//比如 if $var == 1 && $var2 == 2 此时$var,1就是一个对比表达式，代表此Condition生效的条件之一
//Condition_VarName_VarValue_Map即存储了此Condition生效的所有条件
//一个ResourceReplace可以有多个M_Condition，这是正常的，这是因为if会有多个嵌套
//我们每个Condition都代表一个if或else if后的对比表达式的组合，而不考虑嵌套问题，嵌套应该在CommandList解析时进行考虑。

//这个类仅代表一个单独的条件表达式，因为后面多个条件之间还有关系存在
class M_ConditionExpression {
public:
	std::wstring VarName;
	std::wstring VarValue;
};




class M_Condition : public MigotoAttribute {
public:
	//这里分为条件表达式列表，以及一个逻辑连接符列表
	std::vector<M_ConditionExpression> ConditionExpressionList;
	std::vector<std::wstring> LogicList;

	//到时候判断一个Condition是否生效，就要根据传入的变量的值来根据逻辑连接符列表来判断是否生效
	bool isActive(std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap, std::unordered_map<std::wstring, M_Variable> Global_VariableName_M_Variable_Map);

	void show();

	M_Condition();

	M_Condition(std::wstring ConditionStr);
};


class M_Key : public MigotoAttribute {
public:

	M_Condition Condition;
	std::wstring KeyName;
	std::wstring BackName;
	std::wstring Type;

	//例如 $variable5 = 0,1 代表按下这个键之后会让$variable5的值在0,1之间循环切换到下一个，所以是循环变量
	std::unordered_map<std::wstring, std::vector<std::wstring>> CycleVariableName_PossibleValueList_Map;

	//例如$creditinfo = 0 代表按下这个键之后会立刻激活并将$creditinfo设为0，所以是激活变量
	std::unordered_map<std::wstring, std::wstring> ActiveVariableName_ActiveValue_Map;
};


class M_DrawIndexed {
public:
	//例如DrawIndexed = 6,12,0 表示DrawStartIndex为0，DrawOffsetIndex为12，DrawNumber为6
	//即从索引0开始绘制，从Offset开始读取，读取DrawNumber个数量来绘制
	bool AutoDraw = false;
	std::wstring DrawNumber = L"";
	std::wstring DrawOffsetIndex;
	std::wstring DrawStartIndex;

	std::vector<M_Condition> ActiveConditionList;

	M_DrawIndexed();

	std::wstring ToDrawStr() {
		std::wstring DrawStr = L"drawindexed = " + DrawNumber + L"," + DrawOffsetIndex + L"," + DrawStartIndex;
		return DrawStr;
	}
};


class M_ResourceReplace {
public:
	std::wstring ReplaceTarget = L"";
	std::wstring ReplaceResourceName = L"";
	std::vector<M_Condition> ActiveConditionList;

};


class M_TextureOverride : public MigotoAttribute {
public:

	std::wstring IndexBufferHash = L"";
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::wstring MatchPriority = L"";
	std::wstring Handling = L""; //skip
	std::wstring MatchFirstIndex = L"";
	std::vector<M_DrawIndexed> DrawIndexedList;

	//用于处理TextureOverrideIB相关逻辑
	std::wstring IBResourceName = L"";
	std::wstring IBFileName = L"";
	std::wstring IBFilePath = L"";
	std::wstring IBFormat = L"";
	IndexBufferBufFile IBBufFile;

	//用于处理多个Key切换时激活的ResourceReplaceList
	std::vector<M_ResourceReplace> ActiveResourceReplaceList;
};


class M_Resource : public MigotoAttribute {
public:

	std::wstring ResourceName = L"";
	std::wstring Type = L"";
	std::wstring Stride = L"";
	std::wstring Format = L"";
	std::wstring FileName = L"";

	//后面拼接出来的属性，根据NameSpace和FileName
	std::wstring FilePath = L"";

	void Show() {
		LOG.Info(L"ResourceStride:" + Stride);
		LOG.Info(L"ResourceFileName:" + FileName);
		LOG.Info(L"ResourceFilePath:" + FilePath);
	}
};


class M_CommandList : public MigotoAttribute {
public:

	std::wstring CommandListName = L"";
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::vector<M_DrawIndexed> DrawIndexedList;

};