#include "ConfigManager.hpp"

#include <iostream>

#include "Renderer/RenderingManager.hpp"

namespace SP {

	const RadeonRays::float3 ConfigManager::kCameraPos{ -2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraAt{ 2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraUp{ 0.f, 1.1f, 0.f };

	ConfigManager::ConfigManager() : mImageLightField(kNumOfLFs, kNumOfSubLFImgs), mCamera(kCameraPos, kCameraAt, kCameraUp) {

		mChangeSceneFlags.resize(kNumOfLFs * kNumOfSubLFImgs);
		for (size_t i = 0; i < mChangeSceneFlags.size(); ++i) {
			mChangeSceneFlags[i] = false;
		}

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

		const std::size_t totalSz = mImageLightField.getTotalSize();
		std::size_t index = static_cast<std::size_t>(dx * totalSz);

		if (index >= totalSz) {
			index = totalSz - 1;
		}

		return index;
	}

	std::vector<std::size_t> ConfigManager::getIndexArrayOfSubLightField(float dx) const {

		std::vector<std::size_t> indexArray;

		dx += 0.5;
		const std::size_t totalSz = mImageLightField.getTotalSize();
		std::size_t index = static_cast<std::size_t>(dx * totalSz);

		//indexArray.push_back(index);

		std::size_t indexR = 0;
		std::size_t indexL = 0;

		if (index >= totalSz - 1) {
			index = totalSz - 1;
			indexL = index - 1;

			indexArray.push_back(indexL);
			indexArray.push_back(index);

		} else if (index <= 0) {
			index = 0;
			indexR = index + 1;

			indexArray.push_back(index);
			indexArray.push_back(indexR);

		} else {
			indexL = index - 1;
			indexR = index + 1;

			indexArray.push_back(indexL);
			indexArray.push_back(index);
			indexArray.push_back(indexR);
		}

		return indexArray;
	}


	void ConfigManager::clearAll() {
		mImageLightField.clearAll();
	}

	void ConfigManager::saveAll() {
		mImageLightField.saveAll();
	}

	bool ConfigManager::isSceneChanged(size_t index) const {
		return mChangeSceneFlags[index];
	}

	void ConfigManager::setSceneChangedFlag(size_t index, bool flag) {
		mChangeSceneFlags[index] = flag;
	}

	void ConfigManager::setAllSceneChangedFlag(bool flag) {
		for (size_t i = 0; i < mChangeSceneFlags.size(); ++i) {
			mChangeSceneFlags[i] = flag;
		}
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

	CameraConfig ConfigManager::getCameraConfig() const {
		return mCamera;
	}

	std::size_t ConfigManager::getWriteBufferSize() const {
		return kWriteBufferSize;
	}

	void ConfigManager::setRenderManagerPtr(RenderingManager * renManPtr) {
		renderManagerPtr = renManPtr;
	}

	void ConfigManager::resetRenderer() {

		if (renderManagerPtr == nullptr) {
			throw std::runtime_error("renderer not set (null pointer) !");
		}

		renderManagerPtr->reset();

	}

	void ConfigManager::recompileScene() {

		std::cerr << "recompile Scene test" << std::endl;
		renderManagerPtr->recompileScene();


	}




}
