#include "ConfigManager.hpp"

#include <iostream>

#include "Renderer/RenderingManager.hpp"

#include "math/matrix.h"
#include "math/mathutils.h"

#include "math/float3.h"


#include <opencv2/opencv.hpp>

namespace SP {

	const RadeonRays::float3 ConfigManager::kCameraPos { -2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraAt { 2.f, 1.8f, 0.f };
	const RadeonRays::float3 ConfigManager::kCameraUp { 0.f, -1.f, 0.f };

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




	// inverted camera world matrix
	static cv::Matx44f computeViewMatrix(const PerspectiveCamera& camera) {

		const auto& v = camera.getForwardVector();
		const auto& r = camera.getRightVector();
		const auto& u = camera.getUpVector();
		const auto& pos = camera.getPosition();


		const RadeonRays::float3 ip { -dot(r, pos), -dot(u, pos), -dot(v, pos) };

		std::cerr << "In POS: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
		std::cerr << "ip: " << ip.x << " " << ip.y << " " << ip.z << std::endl;


		return cv::Matx44f { r.x, r.y, r.z, ip.x,
							 u.x, u.y, u.z, ip.y,
							 v.x, v.y, v.z, ip.z,
							 0, 0, 0, 1 };
	}

	static cv::Matx44f computeProjectionMatrix(float l, float r, float b, float t, float n, float f) {


		std::cerr << "l, r, n" << l << " "<< r << " " << n << std::endl;

		return cv::Matx44f {
				2 * n / (r - l), 0, (r + l) / (r - l), 0,
				0, 2 * n / (t - b), (t + b) / (t - b), 0,
				0, 0, (n + f) / (n - f), 2 * f * n / (n - f),
				0, 0, -1, 0
		};


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

	const LightField& ConfigManager::getLightField() const {
		return mImageLightField;
	}

	LightField& ConfigManager::getLightField() {
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

	void ConfigManager::setRenderManagerPtr(RenderingManager* renManPtr) {
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


		//std::vector<RadeonRays::matrix> matrixRecords;
		//matrixRecords.resize(getNumberOfCameras());

		// workaround !
		for (size_t i = 0; i < getNumberOfCameras(); ++i) {
			const auto& camData = renderManagerPtr->getPerspectiveCamera(i);        // check this !!!



			// TODO : remember to change to an optimal allocation method

			const cv::Matx44f& viewMat = computeViewMatrix(camData) ;

			const cv::Matx44f& projMat = computeProjectionMatrix(-camData.getSensorSize().x / 2, camData.getSensorSize().x / 2, -camData.getSensorSize().y / 2,
															 camData.getSensorSize().y / 2, camData.getFocusDistance(), camData.getDepthRange().y) ;

			std::cerr << "Pos:" << camData.getPosition().x << " " << camData.getPosition().y << " " << camData.getPosition().z << std::endl;
			std::cerr << "view Mat" << viewMat << std::endl;
			std::cerr << "proj Mat" << projMat << std::endl;



			// ---- for testing only ----


			const auto& camPos = camData.getPosition();

			cv::Matx41f origMat { camPos.x, camPos.y, camPos.z, 1 };

			std::cerr << "orig mat: " << origMat << std::endl;


			// (0, 0, -2) in camera space
			cv::Matx41f newPosMat = viewMat * origMat;
			newPosMat(2, 0) += -2;

			std::cerr << "pos to cam coord: " << newPosMat << std::endl;


			std::cerr << "pos to cam coord to screen projection" << projMat * newPosMat << std::endl;

			//  ---- end of test ----



			std::cerr << "X, Y: " << x << " " << y << " " << std::endl;

			// NDC coord
			float xNDC = 2 * (x / kWidth) - 1;
			float yNDC = 2 * (y / kHeight) - 1;
			std::cerr << "NDC: " << xNDC << " " << yNDC << " " << std::endl;
			cv::Matx41f inputMat { xNDC, yNDC, 0, 1};
			cv::Matx44f transMat = (projMat * viewMat).inv();

			//cv::Mat inputMat(4, 1, CV_32F, inputData);

			//cv::Mat transMat = (projMat * viewMat).inv();

			cv::Matx41f result = transMat * inputMat;

			std::cerr << "test result - world coord : " << result << std::endl;

			//std::cerr << "cam: " << i << " : " << result.x << ' ' << result.y << ' ' << result.z << std::endl;
		}




		//RadeonRays::perspective_proj_fovy_lh_gl();

		// for loop ?
		//renderManagerPtr->changeSceneWithCoordinates(x, y);
	}


}
