/**
 * @file sql_converter.h (incomplete)
 */
#pragma once
#include <vector>
#include <string>
#include <iostream>

const std::string tableName = "dbo.Test";

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
				sqlQuery += "\"" + getDataVector[i] + "\" ";
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
		sqlQuery += conditionVector[i].dataName;
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
	std::cout << sqlQuery << std::endl;
	return sqlQuery;
}