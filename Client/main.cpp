#include "request.h"
#include "protocol_tester.h"
#include "server_connector.h"
#include <sstream>

#define PORT_NUMBER 20000
#define B_1 127//172
#define B_2 0//26
#define B_3 0//98
#define B_4 1//27

void FillDataVector(const std::string&);

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
	//std::string dataString = static_cast<std::string>(client.Message());
	std::string dataString = std::string(client.Message());
	std::cout << client.Message() << std::endl << std::endl;
	FillDataVector(dataString);
	for (const auto& it : DataVector)
		std::cout << it << std::endl;

	Request request;
	request.Description();
	request.DataDescription();

	/*Tester::RequestGenerator requestGenerator;

	std::cout << "Here are some examples.." << std::endl << std::endl;
	for (int i = 0; i < 5; ++i)
		std::cout << requestGenerator.GenerateRequest() << std::endl;*/
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
		
		if (!client.Send(encryptedRequest) || !client.Receive(3000))
		{
			std::cout << "Error: No server..\n";
			break;
		}
		std::cout << std::endl << client.Message() << std::endl;
	}
}

void FillDataVector(const std::string& dataString)
{
	std::string tempStr;
	std::istringstream iss(dataString);
	while (std::getline(iss, tempStr, '#'))
	{
		DataVector.push_back(tempStr);
	}
}