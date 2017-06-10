#include "client_connector.h"
#include <iostream>

#define PORT_NUMBER 20000

void SizePrinter(cis::ClientConnector&, std::ostream&);

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

	cis::ClientConnector server;
	try
	{
		server.Initialize(PORT_NUMBER);
	}
	catch (exc::TCP_Exception tcpExc)
	{
		std::cout << tcpExc.what() << std::endl;
		return -1;
	}

	std::thread sizeThread(SizePrinter, std::ref(server), std::ref(std::cout));

	while (true)
	{
		if (server.Accept())
			std::cout << "Accepted..\n";
		else
			std::cout << "Not accepted..\n";
	}
	sizeThread.join();
}

void SizePrinter(cis::ClientConnector& server, std::ostream& output)
{
	size_t size = -1;
	while (true)
	{
		if (size != server.Size())
		{
			size = server.Size();
			output << "Connected clients: " << size << std::endl;
		}
	}
}