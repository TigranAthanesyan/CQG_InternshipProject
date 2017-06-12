/**
 * @file    db_connector.h
 * @authors Anna Vardzelyan, Tigran Athanesyan
 */
#pragma once
#include "soci.h"
#include "soci-odbc.h"
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <sstream>

namespace cis /// Customer information storage
{
	enum returnType { oneData, manyDatas, count };

	/// Abstract class for connecting to data base
	class IDBConnector
	{
	public:
		virtual std::vector<std::string> SQLRequest(const std::string&, bool) = 0;

	protected:
		std::string m_DSN;
	};

	using IDBConnectorPtr = std::unique_ptr<IDBConnector>;

	/// Class for connecting to data base
	class DBConnector : public IDBConnector
	{
	public:
		DBConnector(std::string i_DSN) { m_DSN = i_DSN; }

		/// Function that takes as argument sql query string
		/// and returns the answer as vector of strings
		std::vector<std::string> SQLRequest(const std::string& sqlRequest, bool isManyFields);
	};

} // namespace