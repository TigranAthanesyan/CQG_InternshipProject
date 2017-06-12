/**
 * @file     protocol_tester.h
 * @authors  Tigran Athanesyan, Anna Vardzelyan
 * @version  1.3
 */

#include "data.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <bitset>

#pragma once

namespace Tester
{

#define TYPE_SIZE 11

	/**
	 * To identify the types of generated phrases
	 * or the end of the generated request
	 */
	enum WordType
	{
		_all,                     /// Keyword "all"
		_quantity,                /// Keyword "quantity"
		_data,                    /// All data names
		_that,                    /// Keyword "that"
		_comma,                   /// Keyword ","
		_is_isNot,                /// Keywords "is" and "is not"
		_isDefined_isUndefined,   /// Keywords "is defined" and "is undefined"
		_and,                     /// Keyword "and"
		_or,                      /// Keyword "or"
		_value,                   /// All values of datas
		_end                      /// End of request
	};

	/**
	 * To identify the types of values
	 * because every value will be different depend on data type
	 *
	 * With time there may be more depending on requirements
	 */
	enum ValueType
	{
		name,     /// Corresponding data types: "last name", "first name", "city", "state", "country"
		e_mail,   /// Corresponding data types: "home e-mail", "work e-mail"
		mr_mrs,   /// Corresponding data type:  "mr. / mrs."
		phone,    /// Corresponding data type:  "phone"
		undefined /// In general cases
	};

	/**
	 * Helper class ValidWordTypeMaker
	 *
	 * Generates correct word type sequence in random order
	 * which later will be used by main class
	 */
	class ValidWordTypeMaker
	{
	public:
		/**
		 * Constructor
		 *
		 * Start state of the bitset
		 * Resolve the first possible word types of the request
		 * "all", "quantity", "data"
		 * 
		 * Call private function resetIndexVector
		 * to turn the index vector to identic state as bitset
		 */
		ValidWordTypeMaker();

		/**
		 * Function GetNextType
		 * 
		 * Returns the one of next possible word types in random order
		 * Sets the bitset and index vector to the valid state for next call
		 */
		WordType GetNextType();

	private:
		/**
		 * Function resetIndexVector
		 *
		 * Clears the index vector and sets the index numbers of bitset which are true
		 * Must be called every time when bitset is changed
		 */
		void resetIndexVector();

		std::bitset<TYPE_SIZE> m_bitSet; /// Contains bool values to show valid word types
		std::vector<int> m_indexVector;  /// Contains the numbers of bitset which are true
		bool m_isConditionPart = false;  /// Turns to true after keyword "that"
	};

	/**
	 * Main class RequestGenerator
	 *
	 * Contains a object of ValidWordTypeMaker
	 * using which generates a correct request 
	 */
	class RequestGenerator
	{
	public:

		/**
		 * Function GenerateRequest
		 *
		 * Calls function GetNextType of typeMaker until it is not "end"
		 * and using that types generate a correct request text
		 */
		std::string GenerateRequest(bool = true);

	private:

		/**
		 * Helper functions for generating valid
		 * data, phone number, e-mail and any word
		 */
		std::string generateData() const;
		std::string generatePhoneNumber(const unsigned = 8) const;
		std::string generateMail() const;
		std::string generateWord(const size_t, const bool = false) const;

		//static std::vector<std::string> m_dataVector; /// Container for storing all possible data types

		ValidWordTypeMaker m_typeMaker;               /// Helper class object for getting valid word types
	};

}