#include <functional>
#include <sstream>
#include "client_connector.h"

namespace
{

/// Function for initializing the data set
std::set<std::string> MakeDataSet()
{
	std::set<std::string> m_dataSet;
	m_dataSet.insert("last name");
	m_dataSet.insert("first name");
	m_dataSet.insert("mr. / mrs.");
	m_dataSet.insert("acct. name");
	m_dataSet.insert("phone");
	m_dataSet.insert("home e-mail");
	m_dataSet.insert("type");
	m_dataSet.insert("industry segment");
	m_dataSet.insert("work e-mail");
	m_dataSet.insert("address");
	m_dataSet.insert("city");
	m_dataSet.insert("state");
	m_dataSet.insert("postal code");
	m_dataSet.insert("country");
	m_dataSet.insert("last contacted date");
	m_dataSet.insert("referral type");
	m_dataSet.insert("referral detail");
	m_dataSet.insert("sales rep.");
	m_dataSet.insert("sales rep. login");
	m_dataSet.insert("company office");
	m_dataSet.insert("trs admin");
	m_dataSet.insert("auth to trade");
	m_dataSet.insert("job title");
	m_dataSet.insert("contact id");
	return std::move(m_dataSet);
}

std::string Parsing(std::vector<cis::TypedWord>& phrases)
{
	/// Parsing
	return std::string();
}

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

		auto decrypted = decrypt(std::string(buffer));
		std::string parsedText = Parsing(decrypted);
		std::string sqlCode = GetSQLCode(parsedText);
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

std::vector<TypedWord> ClientConnector::decrypt(const std::string& encryptedRequest)
{
	enum State
	{
		none,
		positive,
		negative
	};

	std::vector<TypedWord> decryptedRequest;
	std::istringstream iss(encryptedRequest);
	std::string valueWord;
	int type, state;
	char dummy;
	while (iss >> type)
	{

		iss >> dummy;
		if (type != value)
			iss >> state;
		else
			getline(iss, valueWord, dummy);

		switch (type)
		{
		case all:
			decryptedRequest.push_back(TypedWord("all", all));
			break;
		case quantityOf:
			decryptedRequest.push_back(TypedWord("quantity of", quantityOf));
			break;
		case close:
			decryptedRequest.push_back(TypedWord("close", close));
			break;
		case that:
			decryptedRequest.push_back(TypedWord("that", that));
			break;
		case comma:
			decryptedRequest.push_back(TypedWord(",", comma));
			break;
		case is_isNot:
			decryptedRequest.push_back(TypedWord((state == positive ? "is" : "is not"), is_isNot));
			break;
		case isDefined_isUndefined:
			decryptedRequest.push_back(TypedWord((state == positive ? "is defined" : "is undefined"), isDefined_isUndefined));
			break;
		case isMoreThan_isLessThan:
			decryptedRequest.push_back(TypedWord((state == positive ? "is more than" : "is less than"), isMoreThan_isLessThan));
			break;
		case and_:
			decryptedRequest.push_back(TypedWord("and", and_));
			break;
		case or_:
			decryptedRequest.push_back(TypedWord("or", or_));
			break;
		case data:
			int indicator;
			indicator = 0;
			for (auto it = m_dataSet.begin(); it != m_dataSet.end(); ++it, ++indicator)
			{
				if (indicator == state)
				{
					decryptedRequest.push_back(TypedWord(*it, data));
					break;
				}
			}
			break;
		case value:
			decryptedRequest.push_back(TypedWord(valueWord, value));
			break;
		}

		if (!(iss >> dummy))
			break;
	}

	return std::move(decryptedRequest);
}

std::set<std::string> ClientConnector::m_dataSet = MakeDataSet();
}