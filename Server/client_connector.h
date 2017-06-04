#pragma once
#include "network.h"
#include "db_connector.h"
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <mutex>
#include <thread>

namespace cis
{

enum WordType
{
	all,                     /// Keyword "all"
	quantityOf,              /// Keyword "quantity of"
	close,                   /// Keyword "close"
	that,                    /// Keyword "that"
	comma,                   /// Keyword ","
	is_isNot,                /// Keywords "is" and "is not"
	isDefined_isUndefined,   /// Keywords "is defined" and "is undefined"
	isMoreThan_isLessThan,   /// Keywords "is more than" and "is less than"
	and_,                    /// Keyword "and"
	or_,                     /// Keyword "or"
	data,                    /// All data names
	value                    /// All values of datas
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
	std::vector<TypedWord> decrypt(const std::string&);

	SOCKET          m_contactSocket;
	threadMap       m_threads;
	SOCKADDR_IN     m_sockAddr;
	bool            m_isInitialized = false;
	IDBConnectorPtr m_connectorPtr;
	std::mutex      m_mutex;

	static std::set<std::string> m_dataSet;  ///  Container for storing all possible data types
};
}