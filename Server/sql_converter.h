/**
 * @file sql_converter.h (incomplete)
 */
#pragma once
#include <vector>
#include <string>

const std::string tableName = "dbo.AllData";

enum Operation {equal_, notEqual_, is_, isNot_};
enum Conjunction {and_, or_, end_};

/// A struct for designation the conditions separately
struct Condition
{
	Condition(std::string& i_dataName, Operation i_operation, std::string& i_value, Conjunction i_conjunction) :
		dataName(i_dataName), operation(i_operation), value(i_value), conjunction(i_conjunction) {}

	std::string dataName;
	Operation   operation;
	std::string value;
	Conjunction conjunction;
};

std::string GetFieldName(const std::string& data)
{

	std::string retValue;
	char symbol;
	bool isFirstSymbol = true;
	for (int i = 0; i < data.size(); ++i)
	{
		if (isalpha(data[i]))
		{
			char c = data[i];
			if (isFirstSymbol)
			{
				toupper(c);
				isFirstSymbol = false;
			}
			retValue += c;
		}
		else
		{
			isFirstSymbol = true;
		}
	}
	
	return retValue;
}


/// Function that generates SQL Query
/// Does not take into account joins
std::string GetSQLCode(const std::vector<std::string>& getDataVector, const std::vector<Condition>& conditionVector, bool retCount)
{
	std::string sqlQuery = "select ";
	if (retCount)
		sqlQuery += "count (*) ";
	else
	{
		if (!getDataVector.size())
			sqlQuery += "* ";
		else
			for (int i = 0; i < getDataVector.size(); ++i)
			{
				if (i)
					sqlQuery += ", ";
				sqlQuery += "\"" + GetFieldName(getDataVector[i]) + "\" ";
			}
	}
	sqlQuery += "from " + tableName + " where ";
	bool bracketIsOpen = false;
	for (int i = 0; i < conditionVector.size(); ++i)
	{
		if (conditionVector[i].conjunction == or_ && !bracketIsOpen)
		{
			sqlQuery += "(";
			bracketIsOpen = true;
		}
		sqlQuery += GetFieldName(conditionVector[i].dataName);
		switch (conditionVector[i].operation)
		{
		case equal_:
			sqlQuery += " = ";
			break;
		case notEqual_:
			sqlQuery += " != ";
			break;
		case is_:
			sqlQuery += " is ";
			break;
		case isNot_:
			sqlQuery += " is not ";
			break;
		}
		sqlQuery += (conditionVector[i].value == "null" ? "null " : "\'" + conditionVector[i].value + "\' ");
		if (conditionVector[i].conjunction != or_ && bracketIsOpen)
		{
			sqlQuery += ") ";
			bracketIsOpen = false;
		}
		if (conditionVector[i].conjunction != end_)
		{
			sqlQuery += (conditionVector[i].conjunction == and_ ? "and " : "or ");
		}
	}
	return sqlQuery;
}