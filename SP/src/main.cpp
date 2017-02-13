

#include <iostream>

//#include "Common.hpp"

#include "Server.hpp"




int main(int argc, char *argv[]) {

	unsigned port = 8051;
	
	if (argc > 1) {
		port = std::stoi(std::string(argv[1]));
	}

	std::cout << "Server : Listen on Port " << port << std::endl;

	try {
		boost::asio::io_service ios;
		SP::Server server(ios, port);
		ios.run();

		std::cerr << "End Server" << std::endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}




	return 0;
}

