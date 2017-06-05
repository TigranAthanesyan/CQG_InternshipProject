/**
 * @file     server_connector.h
 * @author   Tigran Athanesyan
 * @version  1.3
 */
#pragma once
#include "network.h"

/// Class ServerConnector
/// for connecting with server
class ServerConnector
{
public:
	~ServerConnector();
	void Initialize(short, int, int, int, int); /// Initializes the socket
	bool Connect() noexcept;                    /// Connects to server
	bool Send(const std::string&) noexcept;     /// Sends the string to server
	bool Receive(int) noexcept;                 /// Receives the answer from server
	char* Message() const noexcept;             /// Returns the received answer

private:
	SOCKET*      m_socket = nullptr;     /// Socket for connecting to server
	SOCKADDR_IN  m_sockAddr;             /// An object for binding the socket
	char*        m_buffer = nullptr;     /// A char stream for containing the received message
	bool         m_initialized = false;  /// Bool for initialize socket once
	bool         m_connected = false;    /// Bool for connect to server once
};
