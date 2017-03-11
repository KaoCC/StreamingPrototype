#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>

namespace SP {


	RenderingManager::RenderingManager(SyncBuffer<ImageConfig>& buff) : syncBuffer(buff) {
		//, encoder(CreateEncoder(512, 512)) {

		// allocate renderer
		renderer = std::make_unique<PtRenderer>(1, 5);

		initData();

	}

	RenderingManager::~RenderingManager() {
		if (renderThread) {
			renderThread->join();
		}

		//delete encoder;
	}

	void RenderingManager::startRenderThread() {

		// yet to be done

		std::cerr << "Start Render Thread" << std::endl;

		// testing 
		renderThread = std::make_unique<std::thread>(std::thread(&RenderingManager::testOutput, this, 0));

	}


	// tmp, for testing only
	void RenderingManager::testOutput(int id) {

		// tmp file location
		const std::string pathBase{ "../Resources/SceneImages/" };


		//const std::string kFilePath{ pathBase + "crown_" + (char)('0' + index) + ".ppm" };

		int localCounter = id;

		while (true) {

			int index = localCounter;

			if (localCounter < 20) {
				index = localCounter + 1;
			} else {
				index = 21;
			}

			//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, 10));

			const std::string kFilePath{ pathBase + "crown_" + std::to_string(index) + ".ppm" };
			std::cerr << kFilePath << std::endl;

			try {

				//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath, encoder, accImageBuffer));
				syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath));

				std::this_thread::sleep_for(std::chrono::seconds(5));

				// test
				std::cerr << "Local Counter " << localCounter << '\n';
				//if (localCounter == 10) {
				//	std::ofstream outStream("Hi.txt", std::ifstream::binary);

				//	std::ostream_iterator<char> oi(outStream);
				//	std::copy(accImageBuffer.begin(), accImageBuffer.end(), oi);
				//	std::cerr << "Output" << '\n';

				//}


			} catch (const std::exception &fail) {
				std::cerr << fail.what() << '\n';
			}


			++localCounter;
		}

	}

	void RenderingManager::initData() {


		std::cerr << "init data start" << std::endl;

		// Load obj file

		std::string basepath = defaultPath;
		basepath += "/";
		std::string filename = basepath + defaultModelname;


		// Load OBJ scene
		std::unique_ptr<SP::SceneIO> scene_io(SP::SceneIO::createSceneIO());
		sceneDataPtr = std::unique_ptr<Scene>(scene_io->loadScene(filename, basepath));

		// more here ...


		// for testing
		std::cerr << "init data pass: " << std::endl;
		std::cerr << "number of shapes: " << sceneDataPtr->getNumShapes() << std::endl;
		std::cerr << "number of lights: " << sceneDataPtr->getNumLights() << std::endl;


	}

}
