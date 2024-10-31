#include "MigotoFormat.h"
#include <iostream>
#include <unordered_map>


//--------------------------------------------------------------------------------------------------------------------
void M_ExpressionValue::ParseExpression() {
	//要寻找并分割+ -
	int lastSplitIndex = 0;
	int substrLength = 0;
	for (size_t i = 0; i < this->ExpressionOriginalLine.length(); i++) {
		char ch = this->ExpressionOriginalLine[i];
		if (ch == '+' || ch == '-') {
			//变量名加入
			LOG.Info(L"检测到+ 或 - 运算符");
			substrLength = i - lastSplitIndex;
			std::wstring TmpStr = this->ExpressionOriginalLine.substr(lastSplitIndex, substrLength);
			boost::algorithm::trim(TmpStr);
			TmpStr = MMTString::Remove$Prefix(TmpStr);
			LOG.Info(TmpStr + L"加入表达式计算列表");
			this->ExpressionList.push_back(TmpStr);

			//再把运算符也加入
			std::wstring ex = MMTString::ToWideString(std::string(1, ch));
			this->ExpressionList.push_back(ex);
			LOG.Info(ex + L"加入表达式计算列表");
			//索引位置要+1跳过此字符
			lastSplitIndex = i + 1;
		}
	}
	substrLength = this->ExpressionOriginalLine.length() - lastSplitIndex;
	std::wstring TmpStr = this->ExpressionOriginalLine.substr(lastSplitIndex, substrLength);
	boost::algorithm::trim(TmpStr);
	TmpStr = MMTString::Remove$Prefix(TmpStr);
	this->ExpressionList.push_back(TmpStr);
	LOG.Info(TmpStr + L"加入表达式计算列表");
}



M_ExpressionValue::M_ExpressionValue(std::wstring ExpressionStringLine) {
	this->ExpressionOriginalLine = ExpressionStringLine;
	//每一个变量的值部分都会传递过来进行检测，所以这里就是执行检测逻辑的地方。
	//在这里检测是否为纯变量数值，即检测是否存在+ 和 - 
	if (ExpressionStringLine.find(L"+") != std::wstring::npos) {
		this->isPureValue = false;
	}
	else if (ExpressionStringLine.find(L"-") != std::wstring::npos) {
		this->isPureValue = false;
	}
	else {
		this->isPureValue = true;
	}

	//如果不是纯值的话，就进行表达式解析，放入表达式列表。
	if (!isPureValue) {
		ParseExpression();
	}
}


M_Condition::M_Condition() {




}



std::wstring EvaluateResult(M_ExpressionValue m_expressionValue, std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap) {
	//统一转换为浮点数处理
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
	//暂时处理不了浮点数，只处理int类型，当下足够用就行了。
	return std::to_wstring((int)ExpressionResult);
}



bool M_Condition::isActive(std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap, std::unordered_map<std::wstring, M_Variable> Global_VariableName_M_Variable_Map) {
	//根据key组合，判断其中每一个Expression是否生效
	//这里分几种情况分别进行判断
	//LOG.NewLine();
	//LOG.Info("Start to calculate if condition is active:");
	if (this->LogicList.size() == 0) {
		//LOG.Info("LogicList is empty, only one single expression detected:");
		//没有逻辑符号说明只有一个条件
		M_ConditionExpression condition_expression = this->ConditionExpressionList[0];

		//此时还需要考虑此条件变量是否存在于ActiveKeyValueMap中，如果不存在，则从表达式变量中寻找，现场激活并判断。
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
			LOG.Info(L"此VarName不存在于Key组合中：" + condition_expression.VarName + L" ");
			//如果不存在Key组合中，则需要找到当前M_Variable，并计算出Result
			if (Global_VariableName_M_Variable_Map.contains(condition_expression.VarName.substr(1))) {
				M_Variable m_variable = Global_VariableName_M_Variable_Map[condition_expression.VarName.substr(1)];
				if (m_variable.Type == L"expression") {
					LOG.Info(L"检测到是表达式变量，准备进行表达式校验：");
					std::wstring Result = EvaluateResult(m_variable.ExpressionValue, ActiveKeyValueMap);
					if (Result == condition_expression.VarValue) {
						LOG.Info(L"表达式激活成功");
						return true;
					}
					else {
						LOG.Info(L"表达式激活失败,当前值:" + Result + L" 目标值：" + condition_expression.VarValue);
						LOG.NewLine();
						return false;
					}
				}
				else {
					LOG.Info(L"检测到非表达式变量。");
					if (condition_expression.VarName.substr(1) == L"active") {
						LOG.Info(L"检测到$active变量进行特殊处理，允许通过,激活成功");
						return true;
					}
					else {
						LOG.Info(L"非表达式变量不是$active，不允许通过");
						return false;
					}
				}
			}
			else {
				LOG.Info(L"此VarName也不存在于全局变量列表中，默认激活失败");
				return false;
			}
			
		}
		
	}
	else {
		//LOG.Info("LogicList is not empty, multiple expression detected:");
		//有逻辑符号说明有多个条件，这里又要分多种情况进行处理，一种是只有&&的情况，一种是&& 与 || 并存的情况
		//TODO 由于我们目前遇到的Mod大多数都只使用到了简单的&&逻辑，所以暂时并不支持||

		//查找是否出现了||
		bool findOrLogic = false;
		for (std::wstring LogicStr : this->LogicList) {
			if (LogicStr == L"||") {
				findOrLogic = true;
			}
		}
		if (!findOrLogic) {
			//如果没找到 || 说明所有的Expression都必须生效才行
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
			//在含有||时，如果正好大小为LogicList大小+1，说明全是||，这时候可以直接任意一个生效则整体生效
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

//用于检测并分割字符串中包含&& 或 || 
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


//传入if或者else if后面的条件判断部分字符串，随后解析出其中涉及的变量和逻辑
M_Condition::M_Condition(std::wstring ConditionStr) {

	LOG.NewLine();
	LOG.Info(L"Start to parse condition expression for: " + ConditionStr);
	//首先查找是否含有&& 或者 || 这种类型符号，如果找到了那就按多个进行处理，否则按单个进行处理
	bool singleExpression = true;
	if (ConditionStr.find(L"&&") != std::wstring::npos) {
		singleExpression = false;
	}
	else if (ConditionStr.find(L"||") != std::wstring::npos) {
		singleExpression = false;
	}

	if (singleExpression) {
		LOG.Info("Can't find && or || in ConditionStr, take it as a single expression condition.");
		//首先偷懒，使用= 或 ==分割，如果分割出来的ConditionVarValue里没有再出现== !=或者变量$这类的东西，说明正常运作
		IniLineObject conditionLine(ConditionStr);
		M_ConditionExpression conditionExpression;
		conditionExpression.VarName = conditionLine.LeftStrTrim;
		conditionExpression.VarValue = conditionLine.RightStrTrim;
		this->ConditionExpressionList.push_back(conditionExpression);
	}
	else {
		LOG.Info("find && or || in ConditionStr, take it as a multiple expression condition.");
		//这里要逐个查找&& 或者 ||并进行分割，得到下一部分，再继续分割得到下一部分，直到找不到&& 或 ||为止
		std::vector<std::wstring> result = split_logic_get_expression(ConditionStr);
		for (std::wstring condition_str : result) {
			LOG.Info(L"ConditionStr: " + condition_str);
			//这里要分情况考虑，如果只是单纯的&&，那么完全可以放到Condition列表里
			//但是如果包含||怎么办？这时候两个有一个生效都会导致整个ConditionMap都生效。
			IniLineObject conditionLine(condition_str);
			M_ConditionExpression conditionExpression;
			conditionExpression.VarName = conditionLine.LeftStrTrim;
			conditionExpression.VarValue = conditionLine.RightStrTrim;
			this->ConditionExpressionList.push_back(conditionExpression);
		}
		this->LogicList = split_logic_get_logic(ConditionStr);
	}

	//检查ConditionVarValue中是否含有变量名，逻辑符号等，进行更复杂的解析判断。
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
	//默认使用==，如果==找不到，那就换成=
	std::wstring delimiter = L"==";
	if (firstDoubleEqualIndex == std::wstring::npos) {
		delimiter = L"=";
	}

	//找到了==或者找到了=都可以接受
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

//下面这个留着指定分隔符，上面那个用来判断变量相关的时候
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