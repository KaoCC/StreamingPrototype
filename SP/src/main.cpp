

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

	bool loadRadianceFlag = false;

	if (argc > 2) {
		int flagNum = std::stoi(std::string(argv[2]));
		loadRadianceFlag = (flagNum != 0);
	}


	SP::ConfigManager configMan;

	//SP::LightField imgLightField;
	//SP::SyncBuffer<SP::ImageConfig> imageOutput;

	SP::RenderingManager renderMan(configMan, loadRadianceFlag);

	renderMan.startRenderThread();

	std::cout << "Server : Listen on Port " << port << std::endl;

	try {
		boost::asio::io_service ios;
		SP::Server server(ios, port, configMan);
		ios.run();

		std::cerr << "End Server" << std::endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}




	return 0;
}

