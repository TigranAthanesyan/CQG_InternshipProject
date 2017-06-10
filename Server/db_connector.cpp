#include "db_connector.h"

std::vector<std::string> cis::DBConnector::SQLRequest(const std::string & sqlRequest, bool isManyFields)
{
	std::vector<std::string> retVector;
	try
	{
		soci::connection_parameters param(soci::odbc, "DSN=" + m_DSN);
		soci::session sqlQuery(param);
		soci::rowset<soci::row> rowSet = (sqlQuery.prepare << sqlRequest);
		for (auto const& it : rowSet)
		{
			std::ostringstream oss;
			const soci::row& row = it;
			for (int i = 0; i < static_cast<int>(row.size()); ++i)
			{
				const soci::column_properties& prop = row.get_properties(i);

				if (isManyFields)
					oss << prop.get_name() << ": ";

				if (row.get_indicator(i) == soci::i_null)
				{
					oss << "NULL" << std::endl;
					continue;
				}

				switch (prop.get_data_type())
				{
				case soci::dt_string:
					oss << row.get<std::string>(i);
					break;
				case soci::dt_double:
					oss  << row.get<double>(i);
					break;
				case soci::dt_integer:
					oss << row.get<int>(i);
					break;
				case soci::dt_long_long:
					oss  << row.get<long long>(i);
					break;
				case soci::dt_unsigned_long_long:
					oss << row.get<unsigned long long>(i);
					break;
				case soci::dt_date:
					std::tm when = row.get<std::tm>(i);
					oss << asctime(&when);
					break;
				}
				oss << std::endl;
			}

			retVector.push_back(oss.str());
		}

		return std::move(retVector);
	}
	catch (const soci::soci_error& e)
	{
		std::string errorMessage = "Error: " + static_cast<std::string>(e.what());
		retVector.push_back(errorMessage);
		return std::move(retVector);
	}
	catch (const std::exception& e)
	{
		std::string errorMessage = "Error: " + static_cast<std::string>(e.what());
		retVector.push_back(errorMessage);
		return std::move(retVector);
	}
}
