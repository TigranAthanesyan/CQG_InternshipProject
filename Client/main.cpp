#include "request.h"
#include "protocol_tester.h"
#include "server_connector.h"
#include <sstream>

#define PORT_NUMBER 20000
#define B_1 172
#define B_2 26
#define B_3 98
#define B_4 27

void FillDataVector(const std::string&, std::vector<std::string>&);

int main()
{
	wsa::WSAInitializer wsaInit;
	try
	{
		wsaInit.Initialize();
	}
	catch (exc::TCP_Exception tcpExc)
	{
		std::cout << tcpExc.what() << std::endl;
		return -1;
	}

	ServerConnector client;
	try
	{
		client.Initialize(PORT_NUMBER, B_1, B_2, B_3, B_4);
	}
	catch (exc::TCP_Exception tcpExc)
	{
		std::cout << tcpExc.what() << std::endl;
		return -1;
	}
	if (!client.Connect())
	{
		std::cout << "Can not connect to server.." << std::endl;
		return -1;
	}

	if (!client.Receive(1024))
	{
		std::cout << "Error: No data..\n";
		return -1;
	}
	
	std::string dataString = std::string(client.Message());
	std::vector<std::string> DataVector;

	FillDataVector(dataString, DataVector);

	Request request(&DataVector);
	request.Description();
	request.DataDescription();

	Tester::RequestGenerator requestGenerator(&DataVector);

	std::cout << "Here are some examples.." << std::endl << std::endl;
	for (int i = 0; i < 5; ++i)
		std::cout << requestGenerator.GenerateRequest() << std::endl;
	std::cout << std::endl << std::endl;

	std::string text;
	while (true)
	{
		getline(std::cin, text);
		if (text == "close")
			break;
		request.SetText(text);
		if (!request.IsCorrect())
		{
			std::cout << "Error: " << request.ErrorText() << std::endl << std::endl;
			continue;
		}
		std::string encryptedRequest = request.GetEncryptedText();
		
		std::cout << "Sending, receiving\n";
		if (!client.Send(encryptedRequest) || !client.Receive(6000))
		{
			std::cout << "Error: No server..\n";
			break;
		}
		std::cout << std::endl << client.Message() << std::endl;
	}
}

void FillDataVector(const std::string& dataString, std::vector<std::string>& dataVector)
{
	std::string tempStr;
	std::istringstream iss(dataString);
	while (std::getline(iss, tempStr, '#'))
	{
		dataVector.push_back(tempStr);
	}
}