#include "request.h"
#include "protocol_tester.h"
#include "server_connector.h"
#include <sstream>
#include <thread>
#include <chrono>

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

	std::string text, user, temp;
	int seconds = 0;
	std::cout << "Choose the user" << std::endl
		<< "Press 't' for testing" << std::endl
		<< "Press 'u' for user mode" << std::endl;
	do
	{
		std::getline(std::cin, user);
		if (user == "t")
		{
			std::cout << "Set the interval in seconds between requests: ";
			while (true)
			{
				std::cin >> temp;
				bool isNumber = true;
				for (int i = 0; i < temp.size(); ++i)
				{
					if (!isdigit(temp[i]))
					{
						isNumber = false;
						break;
					}
				}
				if (!isNumber)
				{
					std::cout << "Incorrect number, try again: ";
				}
				else
				{
					seconds = std::stoi(temp);
					break;
				}
			}
		}
		if ((user != "t" && user != "u"))
			std::cout << "Not correct symbol, try again.." << std::endl;
	} while (user != "t" && user != "u");
	

	while (true)
	{
		if (user == "u")
			getline(std::cin, text);
		else
			text = requestGenerator.GenerateRequest(false);
			
		std::cout << text << std::endl;
		if (text == "close")
			break;
		request.SetText(text);
		if (!request.IsCorrect())
		{
			std::cout << "Error: " << request.ErrorText() << std::endl << std::endl;
			continue;
		}
		std::string encryptedRequest = request.GetEncryptedText();
		
		if (!client.Send(encryptedRequest) || !client.Receive(6000))
		{
			std::cout << "Error: No server..\n";
			break;
		}
		std::cout << std::endl << client.Message() << std::endl;
		if (user == "t")
			std::this_thread::sleep_for(std::chrono::seconds(seconds));
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