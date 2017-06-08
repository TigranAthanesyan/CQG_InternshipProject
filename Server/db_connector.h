/**
* @file db_connector.h (incomplete)
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
	class IDBConnector
	{
	public:
		virtual std::vector<std::string> SQLRequest(const std::string& sqlRequest) = 0;

	protected:
		std::string m_DSN;
	};

	using IDBConnectorPtr = std::unique_ptr<IDBConnector>;

	class DBConnector : public IDBConnector
	{
	public:
		DBConnector(std::string i_DSN) { m_DSN = i_DSN; }
		std::vector<std::string> SQLRequest(const std::string& sqlRequest);
	};

} // namespace