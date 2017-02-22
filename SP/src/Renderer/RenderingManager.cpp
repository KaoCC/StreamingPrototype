#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>

namespace SP {

	RenderingManager::RenderingManager() {

		// tmp

		initData();
	}

	void RenderingManager::initData() {


		std::cerr << "init data start" << std::endl;

		// Load obj file

		std::string basepath = defaultPath;
		basepath += "/";
		std::string filename = basepath + defaultModelname;


		// Load OBJ scene
		std::unique_ptr<SP::SceneIO> scene_io(SP::SceneIO::createSceneIO());
		sceneDataPtr.reset(scene_io->loadScene(filename, basepath));

		// more here ...
		

		// for testing
		std::cerr << "init data pass: " << std::endl;
		std::cerr << "number of shapes: " << sceneDataPtr->getNumShapes() <<std::endl;
		std::cerr << "number of lights: " << sceneDataPtr->getNumLights() << std::endl;


	}

}
