/**
 * @file    client_connector.h
 * @author  Tigran Athanesyan
 * @version 1.1
 */
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
	/// To identify words or word's groups
    /// whose behavior needs to consider
	/// for making SQL query
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

	/// A simple structure
	/// that contains a word or phrase
	/// with the type of phrase
	struct TypedWord
	{
		TypedWord(std::string w, WordType wt) : word(w), type(wt) {}
		std::string word;
		WordType type;
	};

	/// Class ClientConnecter
	/// for connecting with clients
	///
	/// Works with many clients threadsafely
	class ClientConnector
	{
		using threadMap = std::map<SOCKET, std::unique_ptr<std::thread>>;
	public:
		~ClientConnector();
		void Initialize(short, int = 1);         /// Initializes contact socket
		bool Accept() noexcept;                  /// Accepts with client and add an element in thread map with socket function
		size_t Size() const noexcept { return m_threads.size(); } /// Returns the client's quantity
		void SocketFunction(SOCKET& i_socket);   /// Function for every client's thread

	private:
		SOCKET          m_contactSocket;         /// Socket for contacting with clients
		threadMap       m_threads;               /// Map that stores all client's sockets as key and thread with function SocketFunction for all of them
		SOCKADDR_IN     m_sockAddr;              /// An object for binding contact socket
		bool            m_isInitialized = false; /// Bool for initialize socket address once
		IDBConnectorPtr m_connectorPtr;          /// An object for snding query to data base
		std::mutex      m_mutex;                 /// A mutex object for sending requests threadsafely
	};
}