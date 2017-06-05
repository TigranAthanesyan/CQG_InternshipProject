///
/// @file    request.h
/// @author  Tigran Athanesyan
///

#pragma once

#include <string>
#include <memory>

namespace cis /// Customer information storage
{
	class IDBConnector
	{
	public:
		virtual void Connect() = 0;

		virtual std::string SQLRequest(const std::string& sqlRequest) = 0;
	};

	using IDBConnectorPtr = std::unique_ptr<IDBConnector>;

} // namespace
