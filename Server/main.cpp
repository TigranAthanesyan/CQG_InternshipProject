#include "client_connector.h"
#include <iostream>

#define PORT_NUMBER 20000

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

	while (true)
	{
		if (server.Accept())
			std::cout << "Accepted..\n";
		else
			std::cout << "Not accepted..\n";
		std::cout << "Connected clients: " << server.Size() << std::endl;
	}
}



/*


all THAT city is London
quantity of city THAT

return values    WHERE      conditions
select count where (select * where City = Moscow, ...)

std::string getSQLstring(std::vector<std::string> get, std::vector<std::string> cond, int )  , bool count, std::string countCond)
{
   if ( get.size() == 0 && count == false)
   std::string SQLText = "select " + " * " + " where " + 
}


*/