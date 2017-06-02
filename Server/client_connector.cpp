#include <functional>
#include "client_connector.h"

namespace
{
std::string GetSQLCode(const std::string& request)
{
	std::string sqlCode{ "select " };
	//////////////////////////////////////
	return sqlCode;
}
}

namespace cis
{
ClientConnector::~ClientConnector()
{
	closesocket(m_contactSocket);
}

void ClientConnector::Initialize(short portNumber, int backLog)
{
	if (m_isInitialized)
	{
		return;
	}

	m_contactSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (m_contactSocket == INVALID_SOCKET)
	{
		throw exc::TCP_Exception("Socket creation failed..");
	}
	m_sockAddr.sin_port = htons(portNumber);
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(m_contactSocket, (SOCKADDR*)&m_sockAddr, sizeof(m_sockAddr)) == SOCKET_ERROR)
	{
		throw exc::TCP_Exception("Socket binding failed..");
	}

	listen(m_contactSocket, backLog);
	m_isInitialized = true;
}

bool ClientConnector::Accept() noexcept
{
	if (!m_isInitialized)
	{
		return false;
	}

	int sockAddrSize = sizeof(m_sockAddr);
	SOCKADDR_IN clientAddr;
	SOCKET tempSocket = SOCKET_ERROR;
	while (tempSocket == SOCKET_ERROR)
	{
		tempSocket = accept(m_contactSocket, (SOCKADDR*)&clientAddr, &sockAddrSize);
	}
	m_threads.emplace(tempSocket, std::make_unique<std::thread>
		(std::thread(std::bind(&ClientConnector::SocketFunction, this, tempSocket))));

	return true;
}

void ClientConnector::SocketFunction(SOCKET& i_socket)
{
	while (true)
	{
		char buffer[512];
		int retVal = recv(i_socket, buffer, 512, NULL);
		if (retVal <= 0 || retVal > 512)
			break;

		std::string sqlCode = GetSQLCode(std::string(buffer));
		m_mutex.lock();
		std::string answer = m_connectorPtr->SQLRequest(sqlCode);
		m_mutex.unlock();
		retVal = send(i_socket, (char*)answer.c_str(), static_cast<int>(answer.size() + 1), NULL);
		if (retVal <= 0 || retVal > answer.size())
			break;
	}
	closesocket(i_socket);
	m_threads.erase(i_socket);
}
}