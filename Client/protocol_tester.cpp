/**
 * @file     protocol_tester.cpp
 * @authors  Tigran Athanesyan, Anna Vardzelyan
 * @version  1.2
 */

#include "protocol_tester.h"
#include <random>

namespace Tester
{

	/// Simple function that returns random number from the range
	int RandomNumber(const int min, const int max)
	{
		std::random_device rd;
		std::mt19937 randomNumber(rd());
		std::uniform_int_distribution<int> range(min, max);
		return range(randomNumber);
	}

	/// Function for initializing the data vector
	std::vector<std::string> MakeDataVector()
	{
		std::vector<std::string> dataVector;
		dataVector.push_back("last name");
		dataVector.push_back("first name");
		dataVector.push_back("mr. / mrs.");
		dataVector.push_back("acct. name");
		dataVector.push_back("phone");
		dataVector.push_back("home e-mail");
		dataVector.push_back("type");
		dataVector.push_back("industry segment");
		dataVector.push_back("work e-mail");
		dataVector.push_back("address");
		dataVector.push_back("city");
		dataVector.push_back("state");
		dataVector.push_back("postal code");
		dataVector.push_back("country");
		dataVector.push_back("last contacted date");
		dataVector.push_back("referral type");
		dataVector.push_back("referral detail");
		dataVector.push_back("sales rep.");
		dataVector.push_back("sales rep. login");
		dataVector.push_back("company office");
		dataVector.push_back("trs admin");
		dataVector.push_back("auth to trade");
		dataVector.push_back("job title");
		dataVector.push_back("contact id");
		return dataVector;
	}

	ValidWordTypeMaker::ValidWordTypeMaker()
		: m_bitSet(std::bitset<TYPE_SIZE>(7)) /// 7 - 00000000111
	{
		resetIndexVector();
	}

	WordType ValidWordTypeMaker::GetNextType()
	{
		if (!m_indexVector.size())
			return _end;

		int nextIndex = RandomNumber(0, static_cast<int>(m_indexVector.size() - 1));
		WordType nextType = static_cast<WordType>(m_indexVector[nextIndex]);

		/// Before returning the type we need to change some settings about which type can be next
		switch (nextType)
		{
		case _all: case _quantity:  /// True types: all, quantity, data
			m_bitSet[_all] = m_bitSet[_quantity] = m_bitSet[_data] = false;
			m_bitSet[_that] = true;
			break;
		case _data:
			m_bitSet[_data] = false;
			if (m_isConditionPart)   /// True types: data
			{
				m_bitSet[_is_isNot] = m_bitSet[_isDefined_isUndefined] = true;
			}
			else   /// True types: all, quantity, data
			{
				m_bitSet[_all] = m_bitSet[_quantity] = m_bitSet[_data] = false;
				m_bitSet[_that] = m_bitSet[_comma] = true;
			}
			break;
		case _that:   /// True types: that, comma
			m_bitSet[_that] = m_bitSet[_comma] = false;
			m_bitSet[_data] = m_isConditionPart = true;
			break;
		case _comma:    /// True types: comma, that
			m_bitSet[_comma] = m_bitSet[_that] = false;
			m_bitSet[_data] = true;
			break;
		case _is_isNot:  /// True types: is_isNot, isDefined_isUndefined
			m_bitSet[_is_isNot] = m_bitSet[_isDefined_isUndefined] = false;
			m_bitSet[_value] = true;
			break;
		case _isDefined_isUndefined:   /// True types: is_isNot, isDefined_isUndefined
			m_bitSet[_is_isNot] = m_bitSet[_isDefined_isUndefined] = false;
			m_bitSet[_and] = m_bitSet[_end] = true;
			break;
		case _and:   /// True types: and, or, end
			m_bitSet[_and] = m_bitSet[_or] = m_bitSet[_end] = false;
			m_bitSet[_data] = true;
			break;
		case _or:   /// True types: and, or, end
			m_bitSet[_and] = m_bitSet[_or] = m_bitSet[_end] = false;
			m_bitSet[_value] = true;
			break;
		case _value:   /// True types: value
			m_bitSet[_value] = false;
			m_bitSet[_and] = m_bitSet[_or] = m_bitSet[_end] = true;
			break;
		case _end:  /// True types: and, or, end
			m_bitSet[_and] = m_bitSet[_or] = m_bitSet[_end] = m_isConditionPart = false;
			m_bitSet[_all] = m_bitSet[_quantity] = m_bitSet[_data] = true;
			break;
		}
		resetIndexVector();
		return nextType;
	}

	void ValidWordTypeMaker::resetIndexVector()
	{
		m_indexVector.clear();
		for (int i = 0; i < TYPE_SIZE; ++i)
		{
			if (m_bitSet[i])
				m_indexVector.push_back(i);
		}
	}

	std::string RequestGenerator::GenerateRequest(bool forExample)
	{
		std::string request;
		WordType nextType = m_typeMaker.GetNextType(); /// Getting the next type
		ValueType nextValueType = undefined;           /// Will be changed after any data
		bool isConditionPart = false;                  /// Will be changed after "that"

		while (nextType != _end)
		{
			switch (nextType)
			{
			case _all:
				request += "all ";
				break;
			case _quantity:
				request += "quantity ";
				break;
			case _data:
			{
				std::string data = generateData();
				request += data + ' ';
				if (isConditionPart)   /// We need to set the value type
				{
					if (data == "last name" || data == "first name" || data == "city" || data == "state" || data == "country")
						nextValueType = name;
					else if (data == "mr. / mrs.")
						nextValueType = mr_mrs;
					else if (data == "phone")
						nextValueType = phone;
					else if (data == "home e-mail" || data == "work e-mail")
						nextValueType = e_mail;
					else
						nextValueType = undefined;
				}
				break;
			}
			case _that:
				request += "that ";
				break;
			case _comma:
				request += ", ";
				break;
			case _is_isNot:
				request += RandomNumber(0, 1) ? "is " : "is not ";
				break;
			case _isDefined_isUndefined:
				request += RandomNumber(0, 1) ? "is defined " : "is undefined ";
				break;
			case _and:
				request += "and ";
				break;
			case _or:
				request += "or ";
				break;
			case _value:
				switch (nextValueType)
				{
				case name:
					if (forExample)
						request += "*** ";
					else
						request += generateWord(RandomNumber(2, 16), true) + ' ';
					break;
				case mr_mrs:
					request += RandomNumber(0, 1) ? "mr " : "mrs ";
					break;
				case phone:
					request += generatePhoneNumber() + ' ';
					break;
				case e_mail:
					if (forExample)
						request += "***@***.com ";
					else
						request += generateMail() + ' ';
					break;
				case undefined:
					if (forExample)
						request += "*** ";
					else
						request += generateWord(RandomNumber(4, 16)) + ' ';
					break;
				}
				break;
			}
			nextType = m_typeMaker.GetNextType();
		}
		return request;
	}

	std::string RequestGenerator::generateData() const
	{
		int dataIndex = RandomNumber(0, static_cast<int>(m_dataVector.size() - 1));
		return m_dataVector[dataIndex];
	}

	std::string RequestGenerator::generatePhoneNumber(const unsigned numberQuantity) const
	{
		int areaCode = RandomNumber(100, 999);
		int phoneNumber = 
			RandomNumber(static_cast<int>(pow(10, numberQuantity - 1)), static_cast<int>(pow(10, numberQuantity) - 1));
		return "+" + std::to_string(areaCode) + " " + std::to_string(phoneNumber);
	}

	std::string RequestGenerator::generateMail() const
	{
		std::string mail;
		const int beforeAtSymbolMaxSize = 20;
		const int beforeDotSymbolMaxSize = 10;

		mail += generateWord(RandomNumber(3, beforeAtSymbolMaxSize)) + '@';
		mail += generateWord(RandomNumber(3, beforeDotSymbolMaxSize)) + '.';
		mail += generateWord(RandomNumber(2, 3));

		return mail;
	}

	std::string RequestGenerator::generateWord(const size_t size, const bool startWithCapital) const // returns random word
	{
		std::string word;
		for (size_t i = 0; i < size; ++i)
			word += RandomNumber(0, 25) + 'a';
		if (startWithCapital)
			word[0] = toupper(word[0]);
		return word;
	}

	std::vector<std::string> RequestGenerator::m_dataVector = MakeDataVector();

}