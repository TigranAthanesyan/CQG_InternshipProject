/**
 * @file     server_connector.cpp
 * @author   Tigran Athanesyan
 * @version  1.3
 */
#include "server_connector.h"

ServerConnector::~ServerConnector()
{
	delete[] m_buffer;
	closesocket(*m_socket);
	delete m_socket;
}

void ServerConnector::Initialize(short portNumber, int b1, int b2, int b3, int b4)
{
	if (!m_initialized)
	{
		m_sockAddr.sin_port = htons(portNumber);
		m_sockAddr.sin_family = AF_INET;
		m_sockAddr.sin_addr.S_un.S_un_b.s_b1 = b1;
		m_sockAddr.sin_addr.S_un.S_un_b.s_b2 = b2;
		m_sockAddr.sin_addr.S_un.S_un_b.s_b3 = b3;
		m_sockAddr.sin_addr.S_un.S_un_b.s_b4 = b4;
	}

	delete m_socket;
	m_socket = new SOCKET;

	*m_socket = socket(AF_INET, SOCK_STREAM, NULL);
	if (*m_socket == INVALID_SOCKET)
	{
		throw exc::TCP_Exception("Socket creation failed..");
	}

	m_initialized = true;
}

bool ServerConnector::Connect() noexcept
{
	if (m_connected)
	{
		return true;
	}

	if (!m_initialized)
	{
		return false;
	}

	m_connected = (connect(*m_socket, (SOCKADDR*)&m_sockAddr, sizeof(m_sockAddr)) == 0);
	return m_connected;
}

bool ServerConnector::Send(const std::string& data) noexcept
{
	if (!m_connected)
		return false;
	int retVal = send(*m_socket, (char*)data.c_str(), data.size() + 1, NULL);
	return retVal == data.size() + 1;
}
bool ServerConnector::Receive(int size) noexcept
{
	if (!m_connected)
	{
		return false;
	}

	if (m_buffer)
		delete[] m_buffer;
	m_buffer = new char[size];

	int retVal = recv(*m_socket, m_buffer, size, NULL);
	return retVal > 0 && retVal <= size;
}

char* ServerConnector::Message() const noexcept
{
	if (m_buffer)
		return m_buffer;
	return (char*)"No received Message..";
}
