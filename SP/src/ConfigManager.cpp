#include "ConfigManager.hpp"

#include <iostream>

const int kWidth = 512;
const int kHeight = 512;

namespace SP {

	ConfigManager::ConfigManager(SyncBuffer<ImageConfig>& buffer) : 
		cameraCfg(Position(0, 100, 10000), Direction(1234, 5678, 1024)), screenCfg(kWidth, kHeight), bufferRef(buffer), encoder(CreateEncoder(kWidth, kHeight)) {

		// KAOCC: TODO: load images

		//loadImages();
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

	void ConfigManager::setPositionDelta(float dx, float dy, float dz) {
		cameraCfg.pos.x += dx;
		cameraCfg.pos.y += dy;
		cameraCfg.pos.z += dz;
	}

	ImageConfig ConfigManager::getImage() {

		// KAOCC: be careful for the first image ! It may receive the default !
		bool status = bufferRef.removeWithTimer(imagePtr, kTimeLimit);

		// for testing
		if (status) {
			std::cerr << "Get image\n";
		} else {
			std::cerr << "Failed to get image, use the cached data\n";
		}

		return *imagePtr;
	}

	ImageConfig ConfigManager::getImageCache() {
		return *imagePtr;
	}

	Encoder * ConfigManager::getEncoder() {
		return encoder;
	}

	ConfigManager::~ConfigManager() {
		delete encoder;
	}

	

	//const ImageConfig & ConfigManager::getImageRef(size_t index) {
	//	return images[index];
	//}

	//void ConfigManager::loadImages() {

	//	// this is just for testing

	//	const size_t numOfImages = 10;

	//	for (size_t i = 0; i < numOfImages; ++i) {

	//		// for testing only
	//		ImageConfig img(16 * i, 512 * 1024);


	//		images.push_back(img);

	//	}

	//}

}
