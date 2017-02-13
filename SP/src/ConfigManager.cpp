#include "ConfigManager.hpp"


namespace SP {

	ConfigManager::ConfigManager() : cameraCfg(Position(0, 100, 10000), Direction(1234, 5678, 1024)), screenCfg(512, 1024) {

		// KAOCC: TODO: load images

		loadImages();
	}

	CameraConfig ConfigManager::getCamera() {
		return cameraCfg;
	}

	void ConfigManager::setScreen(uint32_t w, uint32_t h) {
		screenCfg.width = w;
		screenCfg.height = h;
	}

	ScreenConfig ConfigManager::getScreen() {
		return screenCfg;
	}

	void ConfigManager::setModuleID(uint32_t mID) {
		moduleID = mID;
	}

	const ImageConfig & ConfigManager::getImageRef(size_t index) {
		return images[index];
	}

	void ConfigManager::loadImages() {

		// this is just for testing

		const size_t numOfImages = 10;

		for (size_t i = 0; i < numOfImages; ++i) {

			// for testing only
			ImageConfig img(16 * i, 512 * 1024);


			images.push_back(img);

		}

	}

}
