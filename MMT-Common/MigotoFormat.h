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
// �����Ż����������ͣ�ָ���Ǵ�ini����з�������ȡ�ĵ�һ�������Ϣ������������еķ�������
// ���ڽ���3Dmigoto�Ļ�����������
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
	//�����������ָ���ָ����������Ǹ������жϱ�����ص�ʱ��
	IniLineObject(std::wstring readLine, std::wstring delimiter);
};


class MigotoAttribute {
public:
	//������3Dmigoto��ini��ͨ�����ԣ�����NameSpace�ȵȡ�
	std::wstring NameSpace = L"";
};


//ͨ���࣬��������Mod�ϲ��ͷ�����Ҫ�õ���
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
	//�����﷨ִ����
	/*
	�㷨���£�����ɨ�貢���ÿ��Ԫ�ص��б���
	�ٱ�������б��������������ʽ
	��Ϊ������ʱֻ��Ҫ���Ǳ�������+ - ��������
	�������������ȡ������������������ִ���߼�����Ҫ��ִ��ʱ�洢��һ������
	��Ҫһ��������ִ�з���
	*/
	std::wstring ExpressionOriginalLine;
	std::vector<std::wstring> ExpressionList;
	void ParseExpression();

	//�˱����ڹ��캯��ִ����Ϻ󱻸�ֵ��Ĭ��Ϊfalse
	bool isPureValue = false;
	M_ExpressionValue() {};
	M_ExpressionValue(std::wstring ExpressionStringLine);
};


//ͬʱ����constants��present
class M_Variable : public MigotoAttribute {
public:
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal expression  ��ʹ�õ�ʱ�������Ǳ��ʽ���ͻ�����ͨ���ͣ�����Ǳ��ʽ���ͼǵ�ȥ��������
	M_ExpressionValue ExpressionValue;

	M_Variable();
	//�޳�ʼֵ�ı���
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType);
	//���г�ʼֵ�ı���
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType);

	//���ʽ���͵ı���
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, M_ExpressionValue In_m_ExpressionValue, std::wstring InType);
};


//Condition����һ��������һ��Condition�п����ж���Աȱ��ʽ
//���� if $var == 1 && $var2 == 2 ��ʱ$var,1����һ���Աȱ��ʽ�������Condition��Ч������֮һ
//Condition_VarName_VarValue_Map���洢�˴�Condition��Ч����������
//һ��ResourceReplace�����ж��M_Condition�����������ģ�������Ϊif���ж��Ƕ��
//����ÿ��Condition������һ��if��else if��ĶԱȱ��ʽ����ϣ���������Ƕ�����⣬Ƕ��Ӧ����CommandList����ʱ���п��ǡ�

//����������һ���������������ʽ����Ϊ����������֮�仹�й�ϵ����
class M_ConditionExpression {
public:
	std::wstring VarName;
	std::wstring VarValue;
};




class M_Condition : public MigotoAttribute {
public:
	//�����Ϊ�������ʽ�б��Լ�һ���߼����ӷ��б�
	std::vector<M_ConditionExpression> ConditionExpressionList;
	std::vector<std::wstring> LogicList;

	//��ʱ���ж�һ��Condition�Ƿ���Ч����Ҫ���ݴ���ı�����ֵ�������߼����ӷ��б����ж��Ƿ���Ч
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

	//���� $variable5 = 0,1 �����������֮�����$variable5��ֵ��0,1֮��ѭ���л�����һ����������ѭ������
	std::unordered_map<std::wstring, std::vector<std::wstring>> CycleVariableName_PossibleValueList_Map;

	//����$creditinfo = 0 �����������֮������̼����$creditinfo��Ϊ0�������Ǽ������
	std::unordered_map<std::wstring, std::wstring> ActiveVariableName_ActiveValue_Map;
};


class M_DrawIndexed {
public:
	//����DrawIndexed = 6,12,0 ��ʾDrawStartIndexΪ0��DrawOffsetIndexΪ12��DrawNumberΪ6
	//��������0��ʼ���ƣ���Offset��ʼ��ȡ����ȡDrawNumber������������
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

	//���ڴ���TextureOverrideIB����߼�
	std::wstring IBResourceName = L"";
	std::wstring IBFileName = L"";
	std::wstring IBFilePath = L"";
	std::wstring IBFormat = L"";
	IndexBufferBufFile IBBufFile;

	//���ڴ�����Key�л�ʱ�����ResourceReplaceList
	std::vector<M_ResourceReplace> ActiveResourceReplaceList;
};


class M_Resource : public MigotoAttribute {
public:

	std::wstring ResourceName = L"";
	std::wstring Type = L"";
	std::wstring Stride = L"";
	std::wstring Format = L"";
	std::wstring FileName = L"";

	//����ƴ�ӳ��������ԣ�����NameSpace��FileName
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