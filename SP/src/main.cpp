

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>


//#include "Common.hpp"

#include "Network/Server.hpp"

#include "Renderer/RenderingManager.hpp"


#include "SyncBuffer.hpp"


void producer(SP::SyncBuffer<int>& bufferRef, int id) {

	SP::SyncBuffer<int>::DataPointer a(new int(id));
	std::this_thread::sleep_for(std::chrono::microseconds(id * 10));
	bufferRef.insert(std::move(a));
	//std::this_thread::sleep_for(std::chrono::seconds(id));
}


void testSyncBuffer() {
	//test
	std::vector<std::thread> threads;
	//SP::RenderingManager tmpRendering;


	SP::SyncBuffer<int> buf;
	//SP::SyncBuffer<int>::DataPointer a(new int(10));
	//buf.insert(std::move(a));

	size_t kTestSize = 10;
	for (size_t i = 0; i < kTestSize; ++i) {
		threads.push_back(std::thread(producer, std::ref(buf), i));
	}

	for (size_t i = 0; i < kTestSize; ++i) {
		auto ptr = buf.remove();
		std::cerr << *ptr.get() << std::endl;
	}

	for (size_t i = 0; i < kTestSize; ++i) {
		threads[i].join();
	}
}

int main(int argc, char *argv[]) {

	unsigned port = 8051;
	
	if (argc > 1) {
		port = std::stoi(std::string(argv[1]));
	}


	SP::SyncBuffer<SP::ImageConfig> imageOutput;
	SP::RenderingManager renderMan(imageOutput);

	renderMan.startRenderThread();

	auto imgPtr = imageOutput.remove();

	std::cerr << "Size of SyncBuffer:" << imageOutput.size() << '\n';
	std::cerr << "Size of image: " << imgPtr->getByteSize() << std::endl;

	std::cerr << "Sleep ..." << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(3));

	std::cerr << "Loop over" << '\n';
	for (int i = 0; i < 6; ++i) {
		
		imgPtr = imageOutput.remove();
		std::cerr << "Size of SyncBuffer:" << imageOutput.size() << '\n';
		std::cerr << "Size of image: " << imgPtr->getByteSize() << std::endl;
	}



	// tmp
	int n;
	std::cin >> n;

	// KAOCC: create Thread for Rendering / Encoding

	std::cout << "Server : Listen on Port " << port << std::endl;

	try {
		boost::asio::io_service ios;
		SP::Server server(ios, port, imageOutput);
		ios.run();

		std::cerr << "End Server" << std::endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}




	return 0;
}

