#include "ConfigManager.hpp"

#include <iostream>



namespace SP {

	ConfigManager::ConfigManager() : mImageLightField(kNumOfLFs, kNumOfSubLFImgs) {


	}


	//CameraConfig ConfigManager::getCamera() {
	//	return cameraCfg;
	//}

	//void ConfigManager::setScreen(uint32_t w, uint32_t h) {
	//	screenCfg.width = w;
	//	screenCfg.height = h;
	//}

	//ScreenConfig ConfigManager::getScreen() {
	//	return screenCfg;
	//}

	void ConfigManager::setModuleID(uint32_t mID) {
		moduleID = mID;
	}

	//void ConfigManager::setPositionDelta(float dx, float dy, float dz) {
	//	cameraCfg.pos.x += dx;
	//	cameraCfg.pos.y += dy;
	//	cameraCfg.pos.z += dz;
	//}

	//ImageConfig ConfigManager::getImage() {

	//	// KAOCC: be careful for the first image ! It may receive the default !
	//	bool status = bufferRef.removeWithTimer(imagePtr, kTimeLimit);

	//	// for testing
	//	if (!status) {
	//		std::cerr << "Failed to get image, use the cached data\n";
	//	}

	//	return *imagePtr;
	//}

	//ImageConfig ConfigManager::getImageCache() {
	//	return *imagePtr;
	//}

	size_t ConfigManager::getIndexOfSubLightField(float dx) const {

		dx += 0.5;

		std::size_t totalSz = mImageLightField.getTotalSize();
		std::size_t index = dx * totalSz;

		if (index >= totalSz) {
			index = totalSz - 1;
		}

		return index;
	}


	ImageConfig::ImageBuffer ConfigManager::getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx) {
		return mImageLightField.getSubLightFieldImageWithIndex(subLFIdx, imgIdx);
	}

	bool ConfigManager::getSubLightFieldRefreshState(std::size_t subLFIdx) const {
		return mImageLightField.getSubLightFieldRefreshState(subLFIdx);
	}

	void ConfigManager::setSubLightFieldRefreshState(std::size_t subLFIdx, bool state) {
		mImageLightField.setSubLightFieldRefreshState(subLFIdx, state);
	}

	const LightField & ConfigManager::getLightField() const {
		return mImageLightField;
	}

	LightField & ConfigManager::getLightField() {
		return mImageLightField;
	}

	std::size_t ConfigManager::getNumberOfSubLFs() const {
		return mImageLightField.getTotalSize();
	}

	std::size_t ConfigManager::getNumberOfSubLFImages() const {
		return mImageLightField.getSubLightFieldSize();
	}

	std::size_t ConfigManager::getScreenWidth() const {
		return kWidth;
	}

	std::size_t ConfigManager::getScreenHeight() const {
		return kHeight;
	}


	std::size_t ConfigManager::getNumberOfCameras() const {
		return kNumOfCamera;
	}


	

}
