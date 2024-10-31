#include "MigotoFormat.h"
#include <iostream>
#include <unordered_map>


//--------------------------------------------------------------------------------------------------------------------
void M_ExpressionValue::ParseExpression() {
	//ҪѰ�Ҳ��ָ�+ -
	int lastSplitIndex = 0;
	int substrLength = 0;
	for (size_t i = 0; i < this->ExpressionOriginalLine.length(); i++) {
		char ch = this->ExpressionOriginalLine[i];
		if (ch == '+' || ch == '-') {
			//����������
			LOG.Info(L"��⵽+ �� - �����");
			substrLength = i - lastSplitIndex;
			std::wstring TmpStr = this->ExpressionOriginalLine.substr(lastSplitIndex, substrLength);
			boost::algorithm::trim(TmpStr);
			TmpStr = MMTString::Remove$Prefix(TmpStr);
			LOG.Info(TmpStr + L"������ʽ�����б�");
			this->ExpressionList.push_back(TmpStr);

			//�ٰ������Ҳ����
			std::wstring ex = MMTString::ToWideString(std::string(1, ch));
			this->ExpressionList.push_back(ex);
			LOG.Info(ex + L"������ʽ�����б�");
			//����λ��Ҫ+1�������ַ�
			lastSplitIndex = i + 1;
		}
	}
	substrLength = this->ExpressionOriginalLine.length() - lastSplitIndex;
	std::wstring TmpStr = this->ExpressionOriginalLine.substr(lastSplitIndex, substrLength);
	boost::algorithm::trim(TmpStr);
	TmpStr = MMTString::Remove$Prefix(TmpStr);
	this->ExpressionList.push_back(TmpStr);
	LOG.Info(TmpStr + L"������ʽ�����б�");
}



M_ExpressionValue::M_ExpressionValue(std::wstring ExpressionStringLine) {
	this->ExpressionOriginalLine = ExpressionStringLine;
	//ÿһ��������ֵ���ֶ��ᴫ�ݹ������м�⣬�����������ִ�м���߼��ĵط���
	//���������Ƿ�Ϊ��������ֵ��������Ƿ����+ �� - 
	if (ExpressionStringLine.find(L"+") != std::wstring::npos) {
		this->isPureValue = false;
	}
	else if (ExpressionStringLine.find(L"-") != std::wstring::npos) {
		this->isPureValue = false;
	}
	else {
		this->isPureValue = true;
	}

	//������Ǵ�ֵ�Ļ����ͽ��б��ʽ������������ʽ�б�
	if (!isPureValue) {
		ParseExpression();
	}
}


M_Condition::M_Condition() {




}



std::wstring EvaluateResult(M_ExpressionValue m_expressionValue, std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap) {
	//ͳһת��Ϊ����������
	float ExpressionResult = 0;

	std::wstring lastExpressVal = L"";
	for (std::wstring ExpressVarName : m_expressionValue.ExpressionList) {
		if (ActiveKeyValueMap.contains(L"$" + ExpressVarName)) {
			std::wstring ExpressVal = ActiveKeyValueMap[L"$" + ExpressVarName];
			//LOG.Info(L"ExpressVal: " + ExpressVal);

			if (lastExpressVal == L"") {
				float tmpVal = std::stof(ExpressVal);
				ExpressionResult += tmpVal;
			}
			else if (lastExpressVal == L"+") {
				float tmpVal = std::stof(ExpressVal);
				ExpressionResult += tmpVal;
			}
			else if (lastExpressVal == L"-") {
				float tmpVal = std::stof(ExpressVal);
				ExpressionResult = ExpressionResult - tmpVal;
			}
		}
		else {
			//LOG.Info(L"Operator: " + ExpressVarName);
		}
		lastExpressVal = ExpressVarName;
	}
	//��ʱ�����˸�������ֻ����int���ͣ������㹻�þ����ˡ�
	return std::to_wstring((int)ExpressionResult);
}



bool M_Condition::isActive(std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap, std::unordered_map<std::wstring, M_Variable> Global_VariableName_M_Variable_Map) {
	//����key��ϣ��ж�����ÿһ��Expression�Ƿ���Ч
	//����ּ�������ֱ�����ж�
	//LOG.NewLine();
	//LOG.Info("Start to calculate if condition is active:");
	if (this->LogicList.size() == 0) {
		//LOG.Info("LogicList is empty, only one single expression detected:");
		//û���߼�����˵��ֻ��һ������
		M_ConditionExpression condition_expression = this->ConditionExpressionList[0];

		//��ʱ����Ҫ���Ǵ����������Ƿ������ActiveKeyValueMap�У���������ڣ���ӱ��ʽ������Ѱ�ң��ֳ�����жϡ�
		if (ActiveKeyValueMap.contains(condition_expression.VarName)) {
			if (condition_expression.VarValue == ActiveKeyValueMap[condition_expression.VarName]) {
				LOG.Info(L"Expression VarName: " + condition_expression.VarName);
				LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);
				LOG.Info("Find match key value! expression active!");
				return true;
			}
			else {
				LOG.Info(L"Match Key Value: " + ActiveKeyValueMap[condition_expression.VarName]);
				LOG.Info("Can't find match key value in ActiveKeyValueMap, expression not active!");
				return false;
			}
		}
		else {
			LOG.Info(L"��VarName��������Key����У�" + condition_expression.VarName + L" ");
			//���������Key����У�����Ҫ�ҵ���ǰM_Variable���������Result
			if (Global_VariableName_M_Variable_Map.contains(condition_expression.VarName.substr(1))) {
				M_Variable m_variable = Global_VariableName_M_Variable_Map[condition_expression.VarName.substr(1)];
				if (m_variable.Type == L"expression") {
					LOG.Info(L"��⵽�Ǳ��ʽ������׼�����б��ʽУ�飺");
					std::wstring Result = EvaluateResult(m_variable.ExpressionValue, ActiveKeyValueMap);
					if (Result == condition_expression.VarValue) {
						LOG.Info(L"���ʽ����ɹ�");
						return true;
					}
					else {
						LOG.Info(L"���ʽ����ʧ��,��ǰֵ:" + Result + L" Ŀ��ֵ��" + condition_expression.VarValue);
						LOG.NewLine();
						return false;
					}
				}
				else {
					LOG.Info(L"��⵽�Ǳ��ʽ������");
					if (condition_expression.VarName.substr(1) == L"active") {
						LOG.Info(L"��⵽$active�����������⴦������ͨ��,����ɹ�");
						return true;
					}
					else {
						LOG.Info(L"�Ǳ��ʽ��������$active��������ͨ��");
						return false;
					}
				}
			}
			else {
				LOG.Info(L"��VarNameҲ��������ȫ�ֱ����б��У�Ĭ�ϼ���ʧ��");
				return false;
			}
			
		}
		
	}
	else {
		//LOG.Info("LogicList is not empty, multiple expression detected:");
		//���߼�����˵���ж��������������Ҫ�ֶ���������д���һ����ֻ��&&�������һ����&& �� || ��������
		//TODO ��������Ŀǰ������Mod�������ֻʹ�õ��˼򵥵�&&�߼���������ʱ����֧��||

		//�����Ƿ������||
		bool findOrLogic = false;
		for (std::wstring LogicStr : this->LogicList) {
			if (LogicStr == L"||") {
				findOrLogic = true;
			}
		}
		if (!findOrLogic) {
			//���û�ҵ� || ˵�����е�Expression��������Ч����
			bool allLogicMatch = true;

			//TODO 
			for (M_ConditionExpression condition_expression : this->ConditionExpressionList) {
				//LOG.Info(L"Expression VarName: " + condition_expression.VarName);
				//LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);
				if (condition_expression.VarValue != ActiveKeyValueMap[condition_expression.VarName]) {
					//LOG.Info(L"Match Key Value: " + ActiveKeyValueMap[condition_expression.VarName]);
					allLogicMatch = false;
					break;
				}
			}

			if (allLogicMatch) {
				//LOG.Info("All key value match, condition active.");
				return true;
			}
			else {
				//LOG.Info("Not all key value match in all && logic, condition active fail.");
				return false;
			}


		}
		else {
			//�ں���||ʱ��������ô�СΪLogicList��С+1��˵��ȫ��||����ʱ�����ֱ������һ����Ч��������Ч
			if (this->ConditionExpressionList.size() == this->LogicList.size() + 1) {
				for (M_ConditionExpression condition_expression : this->ConditionExpressionList) {
					LOG.Info(L"Expression VarName: " + condition_expression.VarName);
					LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);

					if (condition_expression.VarValue == ActiveKeyValueMap[condition_expression.VarName]) {
						return true;
					}
				}
			}
			else {
				LOG.Error("Currently Can't Parse && and || in logic expression in the same condition, please contact NicoMico to fix this or waiting for later version update.");
			}
		}
	}
	LOG.NewLine();
	return false;
}

//���ڼ�Ⲣ�ָ��ַ����а���&& �� || 
std::vector<std::wstring> split_logic_get_expression(const std::wstring& expression) {
	std::vector<std::wstring> result;
	std::wstring current;

	for (size_t i = 0; i < expression.length(); i++) {
		if (expression[i] == '&' && expression[i + 1] == '&') {
			if (!current.empty()) {
				boost::algorithm::trim(current);
				result.push_back(current);
				current.clear();
			}
			i++;
		}
		else if (expression[i] == '|' && expression[i + 1] == '|') {
			if (!current.empty()) {
				boost::algorithm::trim(current);
				result.push_back(current);
				current.clear();
			}
			i++;
		}
		else {
			if (expression[i] != '|' && expression[i] != '&') {
				current += expression[i];
			}
		}
	}

	if (!current.empty()) {
		result.push_back(current);
	}

	return result;
}


std::vector<std::wstring> split_logic_get_logic(const std::wstring& expression) {
	std::vector<std::wstring> result;
	std::wstring current;
	for (size_t i = 0; i < expression.length(); i++) {
		if (expression[i] == '&' && expression[i + 1] == '&') {
			result.push_back(L"&&");
			i++;
		}
		else if (expression[i] == '|' && expression[i + 1] == '|') {
			result.push_back(L"||");
			i++;
		}
	}
	return result;
}


//����if����else if����������жϲ����ַ������������������漰�ı������߼�
M_Condition::M_Condition(std::wstring ConditionStr) {

	LOG.NewLine();
	LOG.Info(L"Start to parse condition expression for: " + ConditionStr);
	//���Ȳ����Ƿ���&& ���� || �������ͷ��ţ�����ҵ����ǾͰ�������д������򰴵������д���
	bool singleExpression = true;
	if (ConditionStr.find(L"&&") != std::wstring::npos) {
		singleExpression = false;
	}
	else if (ConditionStr.find(L"||") != std::wstring::npos) {
		singleExpression = false;
	}

	if (singleExpression) {
		LOG.Info("Can't find && or || in ConditionStr, take it as a single expression condition.");
		//����͵����ʹ��= �� ==�ָ����ָ������ConditionVarValue��û���ٳ���== !=���߱���$����Ķ�����˵����������
		IniLineObject conditionLine(ConditionStr);
		M_ConditionExpression conditionExpression;
		conditionExpression.VarName = conditionLine.LeftStrTrim;
		conditionExpression.VarValue = conditionLine.RightStrTrim;
		this->ConditionExpressionList.push_back(conditionExpression);
	}
	else {
		LOG.Info("find && or || in ConditionStr, take it as a multiple expression condition.");
		//����Ҫ�������&& ���� ||�����зָ�õ���һ���֣��ټ����ָ�õ���һ���֣�ֱ���Ҳ���&& �� ||Ϊֹ
		std::vector<std::wstring> result = split_logic_get_expression(ConditionStr);
		for (std::wstring condition_str : result) {
			LOG.Info(L"ConditionStr: " + condition_str);
			//����Ҫ��������ǣ����ֻ�ǵ�����&&����ô��ȫ���Էŵ�Condition�б���
			//�����������||��ô�죿��ʱ��������һ����Ч���ᵼ������ConditionMap����Ч��
			IniLineObject conditionLine(condition_str);
			M_ConditionExpression conditionExpression;
			conditionExpression.VarName = conditionLine.LeftStrTrim;
			conditionExpression.VarValue = conditionLine.RightStrTrim;
			this->ConditionExpressionList.push_back(conditionExpression);
		}
		this->LogicList = split_logic_get_logic(ConditionStr);
	}

	//���ConditionVarValue���Ƿ��б��������߼����ŵȣ����и����ӵĽ����жϡ�
	LOG.Info(L"Parse condition expression over.");
	LOG.NewLine();
}

void M_Condition::show() {

}


//--------------------------------------------------------------------------------------------------------------------
IniLineObject::IniLineObject() {

}

IniLineObject::IniLineObject(std::wstring readLine) {
	int firstDoubleEqualIndex = (int)readLine.find(L"==");
	int firstEqualIndex = (int)readLine.find(L"=");
	//LOG.Info(L"firstDoubleEqualIndex: " + std::to_wstring(firstDoubleEqualIndex));
	//LOG.Info(L"firstEqualIndex: " + std::to_wstring(firstEqualIndex));
	//Ĭ��ʹ��==�����==�Ҳ������Ǿͻ���=
	std::wstring delimiter = L"==";
	if (firstDoubleEqualIndex == std::wstring::npos) {
		delimiter = L"=";
	}

	//�ҵ���==�����ҵ���=�����Խ���
	if (firstEqualIndex != std::wstring::npos || firstDoubleEqualIndex != std::wstring::npos) {
		std::vector<std::wstring> lowerReadLineSplitList = MMTString::SplitStringOnlyMatchFirst(readLine, delimiter);
		if (lowerReadLineSplitList.size() < 2) {
			LOG.Error(L"lowerReadLineSplitList size is " + std::to_wstring(lowerReadLineSplitList.size()) + L",please check!");
		}
		std::wstring leftStr = lowerReadLineSplitList[0];
		std::wstring rightStr = lowerReadLineSplitList[1];
		//LOG.Info(L"leftStr:" + leftStr);
		//LOG.Info(L"rightStr:" + rightStr);
		LeftStr = leftStr;
		RightStr = rightStr;
		boost::algorithm::trim(leftStr);
		boost::algorithm::trim(rightStr);
		LeftStrTrim = leftStr;
		RightStrTrim = rightStr;
		valid = true;
	}
	else {
		LeftStr = L"";
		RightStr = L"";
		LeftStrTrim = L"";
		RightStrTrim = L"";
		valid = false;
	}
}

//�����������ָ���ָ����������Ǹ������жϱ�����ص�ʱ��
IniLineObject::IniLineObject(std::wstring readLine, std::wstring delimiter) {
	int firstEqualIndex = (int)readLine.find_first_of(delimiter);
	if (firstEqualIndex != std::wstring::npos) {
		std::vector<std::wstring> lowerReadLineSplitList = MMTString::SplitStringOnlyMatchFirst(readLine, delimiter);
		std::wstring leftStr = lowerReadLineSplitList[0];
		std::wstring rightStr = lowerReadLineSplitList[1];
		LeftStr = leftStr;
		RightStr = rightStr;
		boost::algorithm::trim(leftStr);
		boost::algorithm::trim(rightStr);
		LeftStrTrim = leftStr;
		RightStrTrim = rightStr;
		valid = true;
	}
	else {
		LeftStr = L"";
		RightStr = L"";
		LeftStrTrim = L"";
		RightStrTrim = L"";
		valid = false;
	}
}




//--------------------------------------------------------------------------------------------------------------------
M_Variable::M_Variable() {

}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->Type = InType;
}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->InitializeValue = InInitializeValue;
	this->Type = InType;
}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, M_ExpressionValue In_m_ExpressionValue, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->Type = InType;
	this->ExpressionValue = In_m_ExpressionValue;
}


//--------------------------------------------------------------------------------------------------------------------
M_DrawIndexed::M_DrawIndexed() {

}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------