/**
 * @file sql_converter.h (incomplete)
 */
#pragma once
#include <vector>
#include <string>

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

/// Function to do
std::string GetSQLCode(const std::vector<std::string>& getDataVector, const std::vector<Condition>& conditionVector, bool retCount)
{
	std::string sqlCode{ "select " };
	//////////////////////////////////////
	return sqlCode;
}


