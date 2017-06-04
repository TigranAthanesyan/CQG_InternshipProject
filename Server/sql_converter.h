#pragma once
#include <vector>
#include <string>

enum _operator {equal, notEqual, is, isNot};
enum conjunction {and, or, null};

struct Condition
{
	std::string dataName;
	_operator oper;
	std::string value;
	conjunction con;
};

std::string GetSQLCode(const std::vector<std::string>& getData, const std::vector<Condition>& condData, bool retCount)
{
	std::string sqlCode{ "select " };
	//////////////////////////////////////
	return sqlCode;
}


