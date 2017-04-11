#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>
#include <chrono>

#include "math/mathutils.h"

#include "../LightField.hpp"

namespace SP {


	RenderingManager::RenderingManager(ConfigManager& cfgRef) : mConfigRef (cfgRef){

		// allocate renderer

		renderFarm.resize(cfgRef.getNumberOfCameras());
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderFarm[i] = std::make_unique<PtRenderer>(1, 5);		// idx, num_of_bounce
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


		const auto& camDefault{ mConfigRef.getCameraConfig() };

		//for (size_t i = 0; i < kNumOfCamera; ++i) {

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
			}

		}

		//}


		// Set Output
		renderOutputData.resize(renderFarm.size());
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderOutputData[i] = renderFarm[i]->createOutput(mConfigRef.getScreenWidth(), mConfigRef.getScreenHeight());
			renderFarm[i]->setOutput(renderOutputData[i]);
		}

	}

	// helper function for rendering
	void RenderingManager::renderingWorker(size_t subLFIdx, size_t subImgIdx) {

		std::cerr << "Worker " << subLFIdx << ' '  << subImgIdx << " starts !\n";

		// test
		ImageConfig img;


		size_t farmIdx = mConfigRef.getNumberOfSubLFs() * subLFIdx + subImgIdx;

		size_t counter = 0;
		const size_t mod = 1;

		while (true) {

			auto t1 = std::chrono::high_resolution_clock::now();
			renderFarm[farmIdx]->render(*sceneDataPtr, farmIdx);
			auto t2 = std::chrono::high_resolution_clock::now();


			if (farmIdx == 0) {
				std::cerr << "Update time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl ;
			}

			// test code
			if (counter % mod == 0) {

				//std::cerr << "----------------- Convert and store ! \n";


				auto& fieldRef = mConfigRef.getLightField();

				img.setId(farmIdx);
				convertOutputToImage(img, farmIdx);
				//img.storeToPPM(counter);

				fieldRef.setSubLightFieldImageWithIndex(subLFIdx, subImgIdx, img);


				// refresh
				fieldRef.setSubLightFieldRefreshState(subLFIdx, true);
			}

			++counter;

		}

	}


	// testing only
	void RenderingManager::convertOutputToImage(ImageConfig & img, size_t outputIdx) {

		const size_t kStride = 3;

		std::vector<RadeonRays::float3> fdata(mConfigRef.getScreenWidth() * mConfigRef.getScreenHeight());
		renderOutputData[outputIdx]->getData(fdata.data());

		ImageConfig::ImageBuffer& imgBufferRef = img.getImageData();

		imgBufferRef.resize(fdata.size() * kStride);

		// tmp gamma
		float gamma = 2.2f;

		for (size_t i = 0; i < fdata.size(); ++i) {
			imgBufferRef[kStride * i] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].x / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
			imgBufferRef[kStride * i + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].y / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
			imgBufferRef[kStride * i + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].z / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
			//imgBufferRef[kStride * i + 3] = 1;
		}


	}
}


