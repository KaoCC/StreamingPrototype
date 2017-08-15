#include "ConfigManager.hpp"

#include <iostream>

#include "Renderer/RenderingManager.hpp"

#include "math/matrix.h"
#include "math/mathutils.h"

#include "math/float3.h"




namespace SP {

	const RadeonRays::float3 ConfigManager::kCameraPos{ -2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraAt{ 2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraUp{ 0.f, -1.f, 0.f };

	ConfigManager::ConfigManager() : mImageLightField { kNumOfLFs, kNumOfSubLFImgs }, mCameraConfig { kCameraPos, kCameraAt, kCameraUp } {

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


	// ----------------------------- Test Area ------------------------------


	// helper function for matrix debugging
	static void printMat(const RadeonRays::matrix& mat) {

		// its a 4*4 matrix
		for (int i = 0; i < 4; ++i) {

			for (int j = 0; j < 4; ++j) {

				std::cerr << "mat[i][j]" << mat.m[i][j] << std::endl;

			}

		}
	}


	//static const RadeonRays::matrix kViewMatrix {1, 0, 0, -1, 0, 1, 0, -2, 0, 0, 1, -3, 0, 0, 0, 1 };


	// helper function for computing the corresponding position from projection space to world space
	// world space coordinate = inv Projection Matrix (with depth info) * ST coordinate
	static RadeonRays::float3 computeProjectionToWorld(float x, float y) {

		RadeonRays::float3 worldPosition;


		// TODO: compute the correct form

		// tmp
		float depth = 0.5f;
		RadeonRays::float3 vec { x, y, depth };

		// matrix ?



		return worldPosition;

	}


	static void computeTransformation(const RadeonRays::float3& vector, RadeonRays::matrix& mat, RadeonRays::matrix& matinv) {


		//TODO: compute the correct form


	}


	// inverted camera world matrix
	static RadeonRays::matrix computeViewMatrix(const PerspectiveCamera& camera) {

		const auto& v = camera.getForwardVector();
		const auto& r = camera.getRightVector();
		const auto& u = camera.getUpVector();
		const auto& pos = camera.getPosition();
		
		const RadeonRays::float3 ip { -dot(r,pos), -dot(u,pos), -dot(v,pos) };

		return RadeonRays::matrix { 
			r.x, r.y, r.z, ip.x,
			u.x, u.y, u.z, ip.y,
			v.x, v.y, v.z, ip.z,
			0, 0, 0, 1 };
	}





	// --------------------------------- End of Test Area ------------------


	size_t ConfigManager::getIndexOfSubLightField(float dx) const {

		dx += 0.5;

		const std::size_t totalSz = mImageLightField.getTotalSize();
		auto index = static_cast<std::size_t>(dx * totalSz);

		if (index >= totalSz) {
			index = totalSz - 1;
		}

		return index;
	}

	std::vector<std::size_t> ConfigManager::getIndexArrayOfSubLightField(float dx) const {

		std::vector<std::size_t> indexArray;

		dx += 0.5;
		const std::size_t totalSz = mImageLightField.getTotalSize();
		auto index = static_cast<std::size_t>(dx * totalSz);

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
		return mCameraConfig;
	}

	std::size_t ConfigManager::getWriteBufferSize() const {
		return kWriteBufferSize;
	}

	void ConfigManager::setRenderManagerPtr(RenderingManager * renManPtr) {
		renderManagerPtr = renManPtr;
	}


	void ConfigManager::enterState(State state) {

		if (renderManagerPtr == nullptr) {
			throw std::runtime_error("renderer not set (null pointer) !");
		}

		renderManagerPtr->reset(state);
	}


	// testing 
	void ConfigManager::changeSceneWithCoordinates(float x, float y) {
		std::cerr << "recompile Scene with ST Plane coordinate" << std::endl;


		std::vector<RadeonRays::matrix> matrixRecords;
		matrixRecords.resize(getNumberOfCameras());

		// workaround !
		for (size_t i = 0; i < getNumberOfCameras(); ++i) {
			const auto& camData = renderManagerPtr->getPerspectiveCamera(i);		// check this !!!


			const RadeonRays::matrix viewMat { computeViewMatrix(camData) };

			matrixRecords[i] = RadeonRays::inverse(
				RadeonRays::perspective_proj_lh_gl(
				-camData.getSensorSize().x / 2, 
				camData.getSensorSize().x / 2, 
				-camData.getSensorSize().y / 2, 
				camData.getSensorSize().y / 2,
				camData.getFocusDistance(), 
				camData.getDepthRange().y) * viewMat);

			
			//RadeonRays::float3 camNewCoord = viewMat *   camData.getPosition();
			//std::cerr << "Get Pos: " << camData.getPosition().x << " " << camData.getPosition().y << std::endl;
			//std::cerr << "camNewCoord:" << i << " : " << camNewCoord.x << " " << camNewCoord.y << " " << camNewCoord.z << std::endl;


			RadeonRays::float4 input { x / kWidth, y / kHeight, 0, 1 };

			RadeonRays::float4 result = matrixRecords[i] * input;

			std::cerr << "cam: " << i << " : " <<result.x << ' ' << result.y << ' ' << result.z << std::endl;
		}


		

		//RadeonRays::perspective_proj_fovy_lh_gl();

		// for loop ?
		//renderManagerPtr->changeSceneWithCoordinates(x, y);
	}


}
