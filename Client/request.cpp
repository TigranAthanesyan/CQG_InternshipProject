/**
* @file    request.cpp
* @author  Tigran Athanesyan
* @version 1.0
*/

#include "request.h"

/// Function for initializing the data set
std::set<std::string> MakeDataSet()
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

void Request::Description(std::ostream& output) const
{
	output << "    Request types.." << std::endl << std::endl
		<< "-------------------------------------------------------------------------" << std::endl
		<< " all that \"condition\" (and \"condition\" ...)" << std::endl
		<< " \"data type 1\" ( , \"data type 2\" , ...)" << std::endl
		<< " \"data type\" that \"condition\" (and \"condition\" ...)" << std::endl
		<< " \"data type 1\" ( , \"data type 2\" , ...) that \"condition\" (and \"condition\"...)" << std::endl
		<< " quantity of \"data type\"" << std::endl
		<< " quantity of \"data type\" that \"condition\" (and \"condition\"...)" << std::endl
		<< " close" << std::endl
		<< "-------------------------------------------------------------------------" << std::endl
		<< std::endl << "    Condition types.." << std::endl << std::endl
		<< "-------------------------------------------------------------------------" << std::endl
		<< " \"data type\" is \"value\" (or \"value\"...)" << std::endl
		<< " \"data type\" is not \"value\" (or \"value\"...)" << std::endl
		<< " \"data type\" is defined" << std::endl
		<< " \"data type\" is undefined" << std::endl
		<< " quantity of \"data type\" is more than \"value\" (or \"value\"...)" << std::endl
		<< " quantity of \"data type\" is less than \"value\" (or \"value\"...)" << std::endl
		<< "-------------------------------------------------------------------------" << std::endl << std::endl;
}

void Request::DataDescription(std::ostream& output) const
{
	output << "    Data types.." << std::endl << std::endl
		<< "-------------------------------------------------------------------------" << std::endl;
	unsigned count = 1;
	for (auto i = m_dataSet.begin(); i != m_dataSet.end(); ++i, ++count)
	{
		output << '\t' << *i;
		if (i->size() < 8)
			output << '\t';
		output << (count % 2 ? '\t' : '\n');
	}
	output << "-------------------------------------------------------------------------" << std::endl << std::endl;
}

void Request::SetText(const std::string& text)
{
	std::vector<std::string> tempArray;
	splitToWords(text, tempArray);
	m_phrases.clear();
	getPhrases(tempArray);
	m_errorText = "";
}

bool Request::IsCorrect()
{
	if (!m_phrases.size())
	{
		m_errorText = "no inputed text";
		return false;
	}

	bool dataIsValid = true, quantityOfIsValid = true, allowComma = true;
	bool thatIsValid = false, commaIsValid = false, valueIsValid = false;
	bool conditionIsValid = false, andIsValid = false, orIsValid = false;
	bool isQuantityOf = false, allowQuantityOf = true, isConditionPart = false;

	for (auto i = m_phrases.begin(); i != m_phrases.end(); ++i)
	{
		switch (i->type)
		{
		case all:
			if (i != m_phrases.begin()) /// can be only at first of expression
			{
				m_errorText = "\"all\" can be only at first of expression";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"all\" is expected condition";
				return false;
			}
			dataIsValid = quantityOfIsValid = allowComma = allowQuantityOf = false;
			thatIsValid = true; /// after can be only "that"
			break;

		case quantityOf:
			if (!quantityOfIsValid || !allowQuantityOf)
			{
				m_errorText = "\"quantity of\" was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"quantity of\" is expected any data name";
				return false;
			}
			quantityOfIsValid = allowComma = allowQuantityOf = false; /// can not be the request like quantity of many data types
			isQuantityOf = true;
			break;

		case data:
			if (!dataIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			dataIsValid = false;
			if (!isConditionPart) ///  before condition part
				thatIsValid = commaIsValid = true; ///  after can be "that" or comma
			else ///  condition part 
			{
				if (i + 1 == m_phrases.end()) /// can not be the last word
				{
					m_errorText = "after \"" + i->word + "\" is expected a continuation of condition";
					return false;
				}
				conditionIsValid = true; /// after can be only condition
			}
			break;

		case that:
			if (!thatIsValid)
			{
				m_errorText = "\"that\" was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"that\" is expected a condition";
				return false;
			}
			thatIsValid = commaIsValid = isQuantityOf = false;
			isConditionPart = dataIsValid = quantityOfIsValid = true; ///  after can be data or "quantity of"
			break;

		case comma:
			if (!allowComma)
			{
				m_errorText = "when requested \"quantity of\" use of comma is not allowed";
				return false;
			}
			if (!commaIsValid)
			{
				m_errorText =  "comma was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after comma is expected a data name";
				return false;
			}
			thatIsValid = commaIsValid = allowQuantityOf = false;
			dataIsValid = true; ///  after can be only data
			break;

		case isDefined_isUndefined:
			if (!conditionIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			if (isQuantityOf)
			{
				m_errorText = "when requested \"quantity of\" use of \"" + i->word + "\" is not allowed";
				return false;
			}
			conditionIsValid = isQuantityOf = false;
			andIsValid = true; ///  after can be only and
			break;

		case is_isNot:
			if (!conditionIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"" + i->word + "\" is expected a value";
				return false;
			}
			conditionIsValid = isQuantityOf = false;
			valueIsValid = true; ///  after can be only value
			break;

		case isMoreThan_isLessThan:
			if (!conditionIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			if (!isQuantityOf)
			{
				m_errorText = "when is not requested \"quantity of\" use of \"" + i->word + "\" is not allowed";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"" + i->word + "\" is expected a value";
				return false;
			}
			conditionIsValid = isQuantityOf = false;
			valueIsValid = true; /// after can be only value
			break;

		case and_:
			if (!andIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"" + i->word + "\" is expected a condition";
				return false;
			}
			andIsValid = orIsValid = false;
			dataIsValid = quantityOfIsValid = true; /// after can be data or quantity of
			break;

		case or_:
			if (!orIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			if (i + 1 == m_phrases.end()) /// can not be the last word
			{
				m_errorText = "after \"" + i->word + "\" is expected a condition";
				return false;
			}
			andIsValid = orIsValid = false;
			valueIsValid = true; /// after can be only value
			break;

		case close:
			if (m_phrases.size() != 1) ///  can be only alone
			{
				m_errorText = "use of \"close\" is allowed only alone";
				return false;
			}
			break;

		case value:
			if (!valueIsValid)
			{
				m_errorText = "\"" + i->word + "\" was used not correctly";
				return false;
			}
			valueIsValid = isQuantityOf = false;
			andIsValid = orIsValid = true; ///  after can be only and/or

			if (((i - 2)->word == "work e-mail" || (i - 2)->word == "home e-mail") && (i - 1)->type == is_isNot && !isEmail(i->word))
			{
				m_errorText = "\"" + i->word + "\" is not correct e-mail";
				return false;
			}
			if ((i - 2)->word == "phone" && (i - 1)->type == is_isNot && !isPhoneNumber(i->word))
			{
				m_errorText = "\"" + i->word + "\" is not correct phone number";
				return false;
			}
			if ((i - 1)->type == isMoreThan_isLessThan && !isNumber(i->word))
			{
				m_errorText = "\"" + i->word + "\" is not correct number";
				return false;
			}
			if ((i - 3)->type == quantityOf && (i - 1)->type == is_isNot && !isNumber(i->word))
			{
				m_errorText = "\"" + i->word + "\" is not correct number";
				return false;
			}
			break;
		}
	}
	return true;
}

bool Request::Close() const
{
	return m_phrases.size() == 1 && m_phrases.back().type == close;
}

std::string Request::ErrorText() const
{
	return m_errorText;
}

std::string Request::GetEncryptedText() const
{
	enum State
	{
		none,
		positive,
		negative
	};

	std::string encryptedRequest;
	for (int i = 0; i < m_phrases.size(); ++i)
	{
		switch (m_phrases[i].type)
		{
		case all:
			encryptedRequest += std::to_string(all) + ',' + std::to_string(none);
			break;
		case quantityOf:
			encryptedRequest += std::to_string(quantityOf) + ',' + std::to_string(none);
			break;
		case close:
			encryptedRequest += std::to_string(close) + ',' + std::to_string(none);
			break;
		case that:
			encryptedRequest += std::to_string(that) + ',' + std::to_string(none);
			break;
		case comma:
			encryptedRequest += std::to_string(comma) + ',' + std::to_string(none);
			break;
		case is_isNot:
			encryptedRequest += std::to_string(is_isNot) + ',' + ((m_phrases[i].word == "is") ? std::to_string(positive) : std::to_string(negative));
			break;
		case isDefined_isUndefined:
			encryptedRequest += std::to_string(isDefined_isUndefined) + ',' + ((m_phrases[i].word == "is defined") ? std::to_string(positive) : std::to_string(negative));
			break;
		case isMoreThan_isLessThan:
			encryptedRequest += std::to_string(isMoreThan_isLessThan) + ',' + ((m_phrases[i].word == "is more than") ? std::to_string(positive) : std::to_string(negative));
			break;
		case and_:
			encryptedRequest += std::to_string(and_) + ',' + std::to_string(none);
			break;
		case or_:
			encryptedRequest += std::to_string(or_) + ',' + std::to_string(none);
			break;
		case data:
			encryptedRequest += std::to_string(data) + ',';
			int indicator;
			indicator = 0;
			for (auto it = m_dataSet.begin(); it != m_dataSet.end(); ++it, ++indicator)
			{
				if (m_phrases[i].word == *it)
				{
					encryptedRequest += std::to_string(indicator);
					break;
				}
			}
			break;
		case value:
			encryptedRequest += std::to_string(value) + ',' + m_phrases[i].word;
			break;
		}

		if (i < m_phrases.size() - 1)
			encryptedRequest += ',';
	}

	return std::move(encryptedRequest);
}

bool Request::isData(const std::string& word) const
{
	return std::find(m_dataSet.begin(), m_dataSet.end(), word) != m_dataSet.end();
}

bool Request::isNumber(const std::string& word) const
{
	for (int i = 0; i < word.size(); ++i)
	{
		if (!isdigit(word[i]))
			return false;
	}
	return true;
}

bool Request::isEmail(const std::string& word) const
{
	auto atIter = std::find(word.begin(), word.end(), '@');
	if (std::count(word.begin(), word.end(), '@') != 1 || atIter == word.begin())
		return false;

	if (std::count(atIter, word.end(), '.') != 1 || word.back() == '.')
		return false;

	if (*word.begin() >= '1' && *word.begin() <= '9')
		return false;

	for (auto i = word.begin(); i != word.end(); ++i)
	{
		if (((*i) >= 'A' && *i <= 'Z') || *i == ' ')
			return false;
	}
	return true;
}

bool Request::isPhoneNumber(const std::string& word) const
{
	if ((word[0] != '+' && !isdigit(word[0])) || word.size() < 10)
		return false;

	bool spaceIsValid = true;
	for (int i = 1; i < word.size(); ++i)
	{
		if (isdigit(word[i]))
		{
			spaceIsValid = true;
			continue;
		}
		if (word[i] == ' ' && spaceIsValid)
		{
			spaceIsValid = false;
			continue;
		}
		return false;
	}
	return true;
}

void Request::splitToWords(const std::string& text, std::vector<std::string>& tempArray) const
{
	std::string temp = "";
	for (int i = 0; i < text.size(); ++i)
	{
		if (text[i] == ' ' || text[i] == ',')
		{
			if (temp != "")
			{
				tempArray.push_back(temp);
				temp = "";
			}
			if (text[i] == ',')
				tempArray.push_back(",");
			continue;
		}
		temp += text[i];
		if (i == text.size() - 1 && temp != "")
			tempArray.push_back(temp);
	}
}

void Request::getPhrases(const std::vector<std::string>& tempArray)
{
	for (auto i = tempArray.begin(); i != tempArray.end(); ++i)
	{
		if (*i == "all")
			m_phrases.push_back(TypedWord(*i, all));
		else if (*i == "that")
			m_phrases.push_back(TypedWord(*i, that));
		else if (*i == "close")
			m_phrases.push_back(TypedWord(*i, close));
		else if (*i == "and")
			m_phrases.push_back(TypedWord(*i, and_));
		else if (*i == "or")
			m_phrases.push_back(TypedWord(*i, or_));
		else if (*i == ",")
			m_phrases.push_back(TypedWord(*i, comma));
		else if (*i == "is")
			m_phrases.push_back(TypedWord(*i, is_isNot));
		else if (*i == "not" && !m_phrases.empty() && m_phrases.back().word == "is")
			m_phrases.back().word += ' ' + *i;
		else if ((*i == "defined" || *i == "undefined") && !m_phrases.empty() && m_phrases.back().word == "is")
		{
			m_phrases.back().word += ' ' + *i;
			m_phrases.back().type = isDefined_isUndefined;
		}
		else if ((*i == "more" || *i == "less") && !m_phrases.empty() && m_phrases.back().word == "is")
		{
			m_phrases.back().word += ' ' + *i;
			m_phrases.back().type = value;
		}
		else if ((*i == "than") && !m_phrases.empty() && (m_phrases.back().word == "is more" || m_phrases.back().word == "is less"))
		{
			m_phrases.back().word += ' ' + *i;
			m_phrases.back().type = isMoreThan_isLessThan;
		}
		else if (*i == "of" && !m_phrases.empty() && m_phrases.back().word == "quantity")
		{
			m_phrases.back().word += ' ' + *i;
			m_phrases.back().type = quantityOf;
		}
		else if (!m_phrases.empty() && (m_phrases.back().type == value || m_phrases.back().type == data))
		{
			m_phrases.back().word += ' ' + *i;
			if (isData(m_phrases.back().word))
				m_phrases.back().type = data;
			else
				m_phrases.back().type = value;
		}
		else
		{
			m_phrases.push_back(TypedWord(*i, value));
			if (isData(*i))
				m_phrases.back().type = data;
		}
	}
}

size_t Request::maximumSize() const
{
	size_t maxSize = 0;
	for (auto i = m_phrases.begin(); i != m_phrases.end(); ++i)
	{
		if (i->word.size() > maxSize)
			maxSize = i->word.size();
	}
	return maxSize;
}

std::set<std::string> Request::m_dataSet = MakeDataSet();