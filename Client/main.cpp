#include "request.h"
#include "protocol_tester.h"

int main()
{
	Request request;
	request.Description();
	request.DataDescription();

	Tester::RequestGenerator requestGenerator;

	std::cout << "Here are some examples.." << std::endl << std::endl;
	for (int i = 0; i < 5; ++i)
		std::cout << requestGenerator.GenerateRequest() << std::endl;
	
	std::cout << std::endl;
	std::cin >> request;
	
	while (!request.Close())
	{
		std::cout << std::endl << request << std::endl << std::endl;
		if (request.IsCorrect())
			std::cout << "Request is correct" << std::endl << std::endl
			<< "Encrypted version: " << request.GetEncryptedText() << std::endl;
		else
			std::cout << "Error: " << request.ErrorText() << std::endl << std::endl;

		std::cin >> request;
	}
}