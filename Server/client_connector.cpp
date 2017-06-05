#include <functional>
#include <sstream>
#include <vector>
#include <set>
#include "client_connector.h"
#include "sql_converter.h"

namespace cis
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

	static std::set<std::string> dataSet = MakeDataSet();  ///  Container for storing all possible data types

	std::vector<TypedWord> Decrypt(const std::string& encryptedRequest)
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
			if (type != _value)
				iss >> state;
			else
				getline(iss, valueWord, dummy);

			switch (type)
			{
			case _all:
				decryptedRequest.push_back(TypedWord("all", _all));
				break;
			case _quantity:
				decryptedRequest.push_back(TypedWord("quantity", _quantity));
				break;
			case _that:
				decryptedRequest.push_back(TypedWord("that", _that));
				break;
			case _comma:
				decryptedRequest.push_back(TypedWord(",", _comma));
				break;
			case _is_isNot:
				decryptedRequest.push_back(TypedWord((state == positive ? "is" : "is not"), _is_isNot));
				break;
			case _isDefined_isUndefined:
				decryptedRequest.push_back(TypedWord((state == positive ? "is defined" : "is undefined"), _isDefined_isUndefined));
				break;
			case _and:
				decryptedRequest.push_back(TypedWord("and", _and));
				break;
			case _or:
				decryptedRequest.push_back(TypedWord("or", _or));
				break;
			case _data:
				int indicator;
				indicator = 0;
				for (auto it = dataSet.begin(); it != dataSet.end(); ++it, ++indicator)
				{
					if (indicator == state)
					{
						decryptedRequest.push_back(TypedWord(*it, _data));
						break;
					}
				}
				break;
			case _value:
				decryptedRequest.push_back(TypedWord(valueWord, _value));
				break;
			}

			if (!(iss >> dummy))
				break;
		}

		return std::move(decryptedRequest);
	}
	void Parse(const std::string& i_encryptedRequest, std::vector<std::string>& o_getDataVector,
		std::vector<Condition>& o_conditionVector, bool& o_retCount)
	{
		auto phrases = Decrypt(i_encryptedRequest);

		o_retCount = phrases[0].type == _quantity;

		int i = 2;
		if (phrases[0].type == _data)
		{
			for (i = 0; i && phrases[i - 1].type != _that; i += 2)
				o_getDataVector.push_back(phrases[i].word);
		}

		while (true)
		{
			std::string dataName = phrases[i].word;
			Operation operation;
			std::string value;
			if (phrases[++i].type == _is_isNot)
			{
				operation = phrases[i].word == "is" ? equal_ : notEqual_;
				value = phrases[++i].word;
			}
			else /// phrases[++i].type == _isDefined_isUndefined
			{
				operation = phrases[i].word == "is defined" ? isNot_ : is_;
				value = "null";
			}
			Conjunction conjunction;
			if (++i = static_cast<int>(phrases.size()))
				conjunction = end_;
			else
				conjunction = phrases[i].type == _and ? and_ : or_;

			o_conditionVector.push_back(Condition(dataName, operation, value, conjunction));

			if (conjunction == end_)
				break;
			++i;
		}
	}

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

			std::string encryptedRequest = std::string(buffer);
			std::vector<std::string> getDataVector;
			std::vector<Condition> conditionVector;
			bool retCount;
			Parse(encryptedRequest, getDataVector, conditionVector, retCount);

			std::string sqlCode = GetSQLCode(getDataVector, conditionVector, retCount);
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