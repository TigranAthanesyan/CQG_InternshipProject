#include "request.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

enum Operation { equal_, notEqual_, is_, isNot_ };
enum Conjunction { and_, or_, end_ };

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

/// Function for initializing the data set
std::set<std::string> MMakeDataSet()
{
	std::set<std::string> m_dataSet;
	m_dataSet.insert("last name");
	m_dataSet.insert("first name");
	m_dataSet.insert("mr. / mrs.");
	m_dataSet.insert("acct. name");
	m_dataSet.insert("phone");
	m_dataSet.insert("home e-mail");
	m_dataSet.insert("type");
	m_dataSet.insert("industry segment");
	m_dataSet.insert("work e-mail");
	m_dataSet.insert("address");
	m_dataSet.insert("city");
	m_dataSet.insert("state");
	m_dataSet.insert("postal code");
	m_dataSet.insert("country");
	m_dataSet.insert("last contacted date");
	m_dataSet.insert("referral type");
	m_dataSet.insert("referral detail");
	m_dataSet.insert("sales rep.");
	m_dataSet.insert("sales rep. login");
	m_dataSet.insert("company office");
	m_dataSet.insert("trs admin");
	m_dataSet.insert("auth to trade");
	m_dataSet.insert("job title");
	m_dataSet.insert("contact id");
	return std::move(m_dataSet);
}

std::set<std::string> dataSet = MMakeDataSet();

/// Function that takes as argument an encrypted request string
/// and turns it into a vector of phrases
std::vector<TypedWord> Decrypt(const std::string& encryptedRequest)
{
	enum State
	{
		none,
		positive,
		negative
	};

	std::vector<TypedWord> decryptedRequest;
	std::istringstream iss(encryptedRequest);
	std::string valueWord;
	int type, state;
	char dummy;
	while (iss >> type)
	{
		iss >> dummy;
		if (type != _value)
			iss >> state;
		else
			getline(iss, valueWord, dummy);

		switch (type)
		{
		case _all:
			decryptedRequest.push_back(TypedWord("all", _all));
			break;
		case _quantity:
			decryptedRequest.push_back(TypedWord("quantity", _quantity));
			break;
		case _that:
			decryptedRequest.push_back(TypedWord("that", _that));
			break;
		case _comma:
			decryptedRequest.push_back(TypedWord(",", _comma));
			break;
		case _is_isNot:
			decryptedRequest.push_back(TypedWord((state == positive ? "is" : "is not"), _is_isNot));
			break;
		case _isDefined_isUndefined:
			decryptedRequest.push_back(TypedWord((state == positive ? "is defined" : "is undefined"), _isDefined_isUndefined));
			break;
		case _and:
			decryptedRequest.push_back(TypedWord("and", _and));
			break;
		case _or:
			decryptedRequest.push_back(TypedWord("or", _or));
			break;
		case _data:
			int indicator;
			indicator = 0;
			for (auto it = dataSet.begin(); it != dataSet.end(); ++it, ++indicator)
			{
				if (indicator == state)
				{
					decryptedRequest.push_back(TypedWord(*it, _data));
					break;
				}
			}
			break;
		case _value:
			decryptedRequest.push_back(TypedWord(valueWord, _value));
			break;
		}

		if (type != _value && !(iss >> dummy))
			break;
	}

	return std::move(decryptedRequest);
}

/// Function for getting the objects that we need
/// for giving them to function GetSQLCode as parameters
void Parse(const std::string& i_encryptedRequest, std::vector<std::string>& o_getDataVector,
	std::vector<Condition>& o_conditionVector, bool& o_retCount)
{
	auto phrases = Decrypt(i_encryptedRequest);

	/// If the first word of request is "quantity"
	/// that meens that we want to get a count
	/// and parameter retCount must be true
	o_retCount = phrases[0].type == _quantity;

	/// If requested all of quantity
	/// index [2] shows the first data name in condition part
	int i = 2;

	/// If requested data names 
	/// we need to put them into getDataVector
	if (phrases[0].type == _data)
	{
		for (i = 0; !i || phrases[i - 1].type != _that; i += 2)
			o_getDataVector.push_back(phrases[i].word);
	}

	/// Index i shows the first data name in condition
	while (true)
	{
		std::string dataName = phrases[i].word;
		Operation operation;
		std::string value;
		if (phrases[++i].type == _is_isNot)
		{
			operation = phrases[i].word == "is" ? equal_ : notEqual_;
			value = phrases[++i].word;
		}
		else /// phrases[++i].type == _isDefined_isUndefined
		{
			operation = phrases[i].word == "is defined" ? isNot_ : is_;
			value = "null";
		}
		Conjunction conjunction;
		if (++i == static_cast<int>(phrases.size()))
			conjunction = end_;
		else
			conjunction = phrases[i].type == _and ? and_ : or_;

		o_conditionVector.push_back(Condition(dataName, operation, value, conjunction));

		while (conjunction == or_)
		{
			value = phrases[++i].word;
			if (++i == static_cast<int>(phrases.size()))
				conjunction = end_;
			else
				conjunction = phrases[i].type == _and ? and_ : or_;
			o_conditionVector.push_back(Condition(dataName, operation, value, conjunction));
		}
		if (conjunction == end_)
			break;
		++i;
	}
}

std::string GetSQLCode(std::vector<std::string>& o_getDataVector, std::vector<Condition>& o_conditionVector, bool& o_retCount)
{
	std::string SQLQuery = "select ";
	if (o_retCount)
		SQLQuery += "count ";
	else
	{
		if (!o_getDataVector.size())
			SQLQuery += "* ";
		else
			for (int i = 0; i < o_getDataVector.size(); ++i)
			{
				if (i)
					SQLQuery += ", ";
				SQLQuery += "\"" + o_getDataVector[i] + "\" ";
			}
	}
	SQLQuery += "where ";
	bool bracketIsOpen = false;
	for (int i = 0; i < o_conditionVector.size(); ++i)
	{
		if (o_conditionVector[i].conjunction == or_ && !bracketIsOpen)
		{
			SQLQuery += "(";
			bracketIsOpen = true;
		}
		SQLQuery += "\"" + o_conditionVector[i].dataName + "\"";
		switch (o_conditionVector[i].operation)
		{
		case equal_:
			SQLQuery += " = ";
			break;
		case notEqual_:
			SQLQuery += " != ";
			break;
		case is_:
			SQLQuery += " is ";
			break;
		case isNot_:
			SQLQuery += " is not ";
			break;
		}
		SQLQuery += (o_conditionVector[i].value == "null" ? "null " : "\"" + o_conditionVector[i].value + "\" ");
		if (o_conditionVector[i].conjunction != or_ && bracketIsOpen)
		{
			SQLQuery += ") ";
			bracketIsOpen = false;
		}
		if (o_conditionVector[i].conjunction != end_)
		{
			SQLQuery += (o_conditionVector[i].conjunction == and_ ? "and " : "or ");
		}
	}
	return SQLQuery;
}
int main()
{
	Request request;
	std::cin >> request;
	while (request.IsCorrect())
	{
		std::string enc = request.GetEncryptedText();
		std::vector<std::string> getDV;
		std::vector<Condition> condV;
		bool retC;
		Parse(enc, getDV, condV, retC);
		std::string sqlCode = GetSQLCode(getDV, condV, retC);
		std::cout << "SQL query: " << sqlCode << std::endl;
		std::cin >> request;
	}
}