/**
 * @file     network.h
 * @author   Tigran Athanesyan
 * @version  1.3
 */
#pragma once

#include <winsock2.h>
#include <stdexcept>
#include <string>

#pragma comment (lib,"WSock32.Lib")

namespace exc
{
	class TCP_Exception : public std::logic_error
	{
	public:
		TCP_Exception(const std::string& what_arg) : std::logic_error(what_arg) {}
	};
}

namespace wsa
{
	class WSAInitializer
	{
	public:
		void Initialize(int version_1 = 2, int version_2 = 2)
		{
			static bool isInitialized = false;
			if (isInitialized)
				return;

			if (WSAStartup(MAKEWORD(version_1, version_2), &m_wsaData) != 0)
			{
				throw exc::TCP_Exception("WSA Initialization failed..");
			}

			isInitialized = true;
		}
		~WSAInitializer()
		{
			WSACleanup();
		}

	private:
		WSADATA m_wsaData;
	};
}
