

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>


//#include "Common.hpp"

#include "Network/Server.hpp"

#include "Renderer/RenderingManager.hpp"


#include "SyncBuffer.hpp"

#include "LightField.hpp"



int main(int argc, char *argv[]) {

	unsigned port = 8051;
	
	if (argc > 1) {
		port = std::stoi(std::string(argv[1]));
	}

	SP::LightField imgLightField;

	SP::SyncBuffer<SP::ImageConfig> imageOutput;
	SP::RenderingManager renderMan(imageOutput, imgLightField);

	renderMan.startRenderThread();

	std::cout << "Server : Listen on Port " << port << std::endl;

	try {
		boost::asio::io_service ios;
		SP::Server server(ios, port, imageOutput, imgLightField);
		ios.run();

		std::cerr << "End Server" << std::endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}




	return 0;
}

