#pragma once
#include "network.h"
#include "db_connector.h"
#include <set>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <thread>

namespace cis
{
class ClientConnector
{
	using threadMap = std::map<SOCKET, std::unique_ptr<std::thread>>;
public:
	~ClientConnector();
	void Initialize(short, int = 1);
	bool Accept() noexcept;
	size_t Size() const noexcept { return m_threads.size(); }

	void SocketFunction(SOCKET& i_socket);

private:
	SOCKET          m_contactSocket;
	threadMap       m_threads;
	SOCKADDR_IN     m_sockAddr;
	bool            m_isInitialized = false;
	IDBConnectorPtr m_connectorPtr;
	std::mutex      m_mutex;
};
}