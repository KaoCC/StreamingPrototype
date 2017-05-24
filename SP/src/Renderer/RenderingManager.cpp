#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>
#include <chrono>

#include "math/mathutils.h"

#include "../LightField.hpp"

#include <cstdio>

namespace SP {


	RenderingManager::RenderingManager(ConfigManager& cfgRef) : mConfigRef (cfgRef){


		// Get device / backend info

		int cpuIdx = -1;
		int gpuIdx = -1;
		int embreeIdx = -1;

		// Always use Embree
		RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

		for (auto idx = 0U; idx < RadeonRays::IntersectionApi::GetDeviceCount(); ++idx) {
			RadeonRays::DeviceInfo devinfo;
			RadeonRays::IntersectionApi::GetDeviceInfo(idx, devinfo);

			// KAOCC: device platform is bugged?
			std::printf( "DeviceInfo: [%s] [%s] [%i] [%x]\n", devinfo.name, devinfo.vendor, devinfo.type, devinfo.platform );

			if ( devinfo.type == RadeonRays::DeviceInfo::kCpu && cpuIdx == -1) {
				cpuIdx = idx;
			}

			if (devinfo.type == RadeonRays::DeviceInfo::kGpu && gpuIdx == -1) {
				gpuIdx = idx;
			}

			if (devinfo.platform == RadeonRays::DeviceInfo::kEmbree && devinfo.type == RadeonRays::DeviceInfo::kCpu && embreeIdx == -1) {
				embreeIdx = idx;
			}
		}


		int nativeIdx = -1;

		// select order:  GPU > Embree > CPU
		if (gpuIdx != -1) {
			nativeIdx = gpuIdx;
		} else if (embreeIdx != -1) {
			nativeIdx = embreeIdx;
		} else if (cpuIdx != -1) {
			nativeIdx = cpuIdx;
		}

		std::cerr << "Selected Device ID: " << nativeIdx << std::endl;

		// allocate renderer
		renderFarm.resize(cfgRef.getNumberOfCameras());
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderFarm[i] = std::make_unique<PtRenderer>(nativeIdx, 5);		// idx, num_of_bounce
		}

		renderThreads.resize(renderFarm.size());

		initData();
	}

	RenderingManager::~RenderingManager() {

		for (auto& renderer : renderThreads) {
			if (renderer) {
				renderer->join();
			}
		}

		//delete encoder;


		// delete output
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderFarm[i]->deleteOutput(renderOutputData[i]);
		}
	}

	void RenderingManager::startRenderThread() {

		// yet to be done

		std::cerr << "Start Render Thread" << std::endl;

		// testing 
		//renderThread = std::make_unique<std::thread>(std::thread(&RenderingManager::testOutput, this, 0));

		//for (size_t i = 0; i < renderFarm.size(); ++i) {
		//	renderThreads[i] = std::make_unique<std::thread>(std::thread(&RenderingManager::renderingWorker, this, i));
		//}


		for (size_t i = 0; i < mConfigRef.getNumberOfSubLFs(); ++i) {
			for (size_t j = 0; j < mConfigRef.getNumberOfSubLFImages(); ++j) {
				renderThreads.push_back(std::make_unique<std::thread>(std::thread(&RenderingManager::renderingWorker, this, i, j)));
			}

		}
	}


	// tmp, for testing only
	//void RenderingManager::testOutput(int id) {

	//	// tmp file location
	//	const std::string pathBase{ "../Resources/LF/" };

	//	//const std::string kFilePath{ pathBase + "crown_" + (char)('0' + index) + ".ppm" };

	//	int localCounter = id;
	//	while (true) {

	//		int index = localCounter % 6;

	//		//if (localCounter < 5) {
	//		//	index = localCounter + 1;
	//		//} else {
	//		//	index = 0;
	//		//}

	//		//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, 10));

	//		const std::string kFilePath{ pathBase + "LF_crown_" + std::to_string(index) + ".ppm" };
	//		std::cerr << kFilePath << std::endl;

	//		try {

	//			//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath, encoder, accImageBuffer));
	//			syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath));
	//			syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath));

	//			std::this_thread::sleep_for(std::chrono::milliseconds(500));

	//			// test
	//			std::cerr << "Local Counter " << localCounter << '\n';
	//			//if (localCounter == 10) {
	//			//	std::ofstream outStream("Hi.txt", std::ifstream::binary);

	//			//	std::ostream_iterator<char> oi(outStream);
	//			//	std::copy(accImageBuffer.begin(), accImageBuffer.end(), oi);
	//			//	std::cerr << "Output" << '\n';

	//			//}

	//		} catch (const std::exception &fail) {
	//			std::cerr << fail.what() << '\n';
	//		}


	//		++localCounter;
	//	}

	//}

	void RenderingManager::initData() {


		std::cerr << "init data start" << std::endl;


		// rand
		RadeonRays::rand_init();

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


		// Set Output
		renderOutputData.resize(renderFarm.size());
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderOutputData[i] = renderFarm[i]->createOutput(mConfigRef.getScreenWidth(), mConfigRef.getScreenHeight());
			renderFarm[i]->setOutput(renderOutputData[i]);
		}


		auto& fieldRef{ mConfigRef.getLightField() };

		const auto& camDefault{ mConfigRef.getCameraConfig() };

		for (size_t i = 0; i < mConfigRef.getNumberOfSubLFs(); ++i) {

			for (size_t j = 0; j < mConfigRef.getNumberOfSubLFImages(); ++j) {

				// KAOCC: TODO: add camera config
				auto* cameraPtr = new PerspectiveCamera(camDefault.mCameraPos + RadeonRays::float3(kStep * i, kStep * j, 0), camDefault.mCameraAt + RadeonRays::float3(kStep * i, kStep * j, 0), camDefault.mCameraUp);
				sceneDataPtr->attachCamera(cameraPtr);

				// Adjust sensor size based on current aspect ratio
				float aspect = (float)mConfigRef.getScreenWidth() / mConfigRef.getScreenHeight();
				g_camera_sensor_size.y = g_camera_sensor_size.x / aspect;

				cameraPtr->setSensorSize(g_camera_sensor_size);
				cameraPtr->setDepthRange(g_camera_zcap);
				cameraPtr->setFocalLength(g_camera_focal_length);
				cameraPtr->setFocusDistance(g_camera_focus_distance);
				cameraPtr->setAperture(g_camera_aperture);

				std::cout << "Camera type: " << (cameraPtr->getAperture() > 0.f ? "Physical" : "Pinhole") << "\n";			// This might cause problems
				std::cout << "Lens focal length: " << cameraPtr->getFocalLength() * 1000.f << "mm\n";
				std::cout << "Lens focus distance: " << cameraPtr->getFocusDistance() << "m\n";
				std::cout << "F-Stop: " << 1.f / (cameraPtr->getAperture() * 10.f) << "\n";
				std::cout << "Sensor size: " << g_camera_sensor_size.x * 1000.f << "x" << g_camera_sensor_size.y * 1000.f << "mm\n";

				// test !
				sceneDataPtr->attachAutoreleaseObject(cameraPtr);

				
				// Link to RenderOutput
				
				//fieldRef.setSubLightFieldRadianceWithIndex(i, j, dynamic_cast<RenderOutput*>(renderOutputData[mConfigRef.getNumberOfSubLFImages() * i + j]));

				fieldRef[i][j].setRadiancePtr(dynamic_cast<RenderOutput*>(renderOutputData[mConfigRef.getNumberOfSubLFImages() * i + j]));

			}

		}


	}

	// helper function for rendering
	void RenderingManager::renderingWorker(size_t subLFIdx, size_t subImgIdx) {

		std::cerr << "Worker " << subLFIdx << ' '  << subImgIdx << " starts !\n";

		// test
		//ImageConfig img;

		size_t farmIdx = mConfigRef.getNumberOfSubLFImages() * subLFIdx + subImgIdx;

		size_t counter = 0;
		const size_t mod = 1;

		while (true) {

			auto t1 = std::chrono::high_resolution_clock::now();
			renderFarm[farmIdx]->render(*sceneDataPtr, farmIdx);
			auto t2 = std::chrono::high_resolution_clock::now();

			if (farmIdx == 0) {
				std::cerr << "Update time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl ;
			}

			// tmp, need lock , need interrupt-basde method
			if (mConfigRef.isSceneChanged(farmIdx)) {
				renderFarm[farmIdx]->clear(0.f, *(renderOutputData[farmIdx]));
				mConfigRef.setSceneChangedFlag(farmIdx, false);
			}

			// test code
			if (counter % mod == 0) {

				//std::cerr << "----------------- Convert and store ! \n";

				auto& fieldRef = mConfigRef.getLightField();

				// remove ?
				fieldRef[subLFIdx][subImgIdx].setId(farmIdx);

				//fieldRef[subLFIdx].setRefreshFlag(true);
				fieldRef[subLFIdx][subImgIdx].setRefreshState(true);
			}

			++counter;

		}

	}


	// testing only
	// should be changed
	//void RenderingManager::convertOutputToImage(ImageConfig & img, size_t outputIdx) {

	//	const size_t kStride = 3;

	//	const size_t screenWidth = mConfigRef.getScreenWidth();
	//	const size_t screenHeight = mConfigRef.getScreenHeight();

	//	std::vector<RadeonRays::float3> fdata(mConfigRef.getScreenWidth() * mConfigRef.getScreenHeight());
	//	renderOutputData[outputIdx]->getData(fdata.data());

	//	ImageConfig::ImageBuffer& imgBufferRef = img.getImageData();

	//	imgBufferRef.resize(fdata.size() * kStride);

	//	// tmp gamma
	//	const float gamma = 2.2f;

	//	//for (size_t i = 0; i < fdata.size(); ++i) {
	//	//	imgBufferRef[kStride * i] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].x / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
	//	//	imgBufferRef[kStride * i + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].y / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
	//	//	imgBufferRef[kStride * i + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].z / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
	//	//	//imgBufferRef[kStride * i + 3] = 1;
	//	//}

	//	size_t currentindex = 0;

	//	for (size_t y = 0; y < screenHeight; ++y) {
	//		for (size_t x = 0; x < screenWidth; ++x) {

	//			const RadeonRays::float3& val = fdata[(screenHeight - 1 - y) * screenWidth + x];

	//			imgBufferRef[currentindex] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.x / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
	//			imgBufferRef[currentindex + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.y / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
	//			imgBufferRef[currentindex + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.z / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));

	//			currentindex += kStride;
	//		}

	//	}


	//}
}


