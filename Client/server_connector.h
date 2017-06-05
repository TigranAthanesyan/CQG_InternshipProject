/**
 * @file     server_connector.h
 * @author   Tigran Athanesyan
 * @version  1.3
 */
#pragma once
#include "network.h"

class ServerConnector
{
public:
	~ServerConnector();
	void Initialize(short, int, int, int, int);
	bool Connect() noexcept;
	bool Send(const std::string&) noexcept;
	bool Receive(int) noexcept;
	char* Message() const noexcept;

private:
	SOCKET*      m_socket = nullptr;
	SOCKADDR_IN  m_sockAddr;
	char*        m_buffer = nullptr;
	bool         m_initialized = false;
	bool         m_connected = false;
};
