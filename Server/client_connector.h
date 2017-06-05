#pragma once
#include "network.h"
#include "db_connector.h"
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <thread>

namespace cis
{
	enum WordType
	{
		_all,                     /// Keyword "all"
		_quantity,                /// Keyword "quantity"
		_that,                    /// Keyword "that"
		_comma,                   /// Keyword ","
		_is_isNot,                /// Keywords "is" and "is not"
		_isDefined_isUndefined,   /// Keywords "is defined" and "is undefined"
		_and,                     /// Keyword "and"
		_or,                      /// Keyword "or"
		_data,                    /// All data names
		_value                    /// All values of datas
	};

	struct TypedWord
	{
		TypedWord(std::string w, WordType wt) : word(w), type(wt) {}
		std::string word;
		WordType type;
	};

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