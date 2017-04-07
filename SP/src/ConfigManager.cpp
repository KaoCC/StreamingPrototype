#include "ConfigManager.hpp"

#include <iostream>

const int kWidth = 512;
const int kHeight = 512;

namespace SP {

	ConfigManager::ConfigManager(SyncBuffer<ImageConfig>& buffer, LightField& imgLF) : 
		cameraCfg(Position(0, 1, 3), Direction(0, 0, 1)), screenCfg(kWidth, kHeight), bufferRef(buffer), imageLightFieldRef(imgLF), encoder(CreateEncoder(kWidth, kHeight)) {

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
		if (!status) {
			std::cerr << "Failed to get image, use the cached data\n";
		}

		return *imagePtr;
	}

	ImageConfig ConfigManager::getImageCache() {
		return *imagePtr;
	}

	size_t ConfigManager::getSubLightFieldSize(size_t subLFIdx) const {
		return imageLightFieldRef.getSubLightFieldSize(subLFIdx);
	}

	ImageConfig::ImageBuffer ConfigManager::getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx) {
		return imageLightFieldRef.getSubLightFieldImageWithIndex(subLFIdx, imgIdx);
	}

	bool ConfigManager::getSubLightFieldRefreshState(std::size_t subLFIdx) const {
		return imageLightFieldRef.getSubLightFieldRefreshState(subLFIdx);
	}

	void ConfigManager::setSubLightFieldRefreshState(std::size_t subLFIdx, bool state) {
		imageLightFieldRef.setSubLightFieldRefreshState(subLFIdx, state);
	}

	Encoder * ConfigManager::getEncoder() {
		return encoder;
	}

	ConfigManager::~ConfigManager() {
		delete encoder;
	}

	

}
