#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>

namespace SP {


	RenderingManager::RenderingManager(SyncBuffer<ImageConfig>& buff) : syncBuffer(buff){

		// allocate renderer
		renderer = std::make_unique<PtRenderer>(1, 5);

		initData();

	}

	RenderingManager::~RenderingManager() {
		if (renderThread) {
			renderThread->join();
		}
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
		const std::string kFilePath{ "../Resources/SceneImages/Crown_rev_2.png" };

		int localCounter = id;
		while (true) {

			//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, 10));


			syncBuffer.insert(std::make_unique<ImageConfig>(kFilePath));


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
		std::cerr << "number of shapes: " << sceneDataPtr->getNumShapes() <<std::endl;
		std::cerr << "number of lights: " << sceneDataPtr->getNumLights() << std::endl;


	}

}
