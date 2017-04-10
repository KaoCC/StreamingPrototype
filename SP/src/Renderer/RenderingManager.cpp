#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>

#include "math/mathutils.h"

#include "../LightField.hpp"

namespace SP {


	RenderingManager::RenderingManager(SyncBuffer<ImageConfig>& buff, LightField& lf) : syncBuffer(buff), imageLightField(lf) {
		//, encoder(CreateEncoder(512, 512)) {

		// allocate renderer

		renderFarm.resize(kNumOfCamera); // tmp
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

		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderThreads[i] = std::make_unique<std::thread>(std::thread(&RenderingManager::renderingWorker, this, i));
		}
	}


	// tmp, for testing only
	void RenderingManager::testOutput(int id) {

		// tmp file location
		const std::string pathBase{ "../Resources/LF/" };


		//const std::string kFilePath{ pathBase + "crown_" + (char)('0' + index) + ".ppm" };

		int localCounter = id;

		while (true) {

			int index = localCounter % 6;

			//if (localCounter < 5) {
			//	index = localCounter + 1;
			//} else {
			//	index = 0;
			//}

			//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, 10));

			const std::string kFilePath{ pathBase + "LF_crown_" + std::to_string(index) + ".ppm" };
			std::cerr << kFilePath << std::endl;

			try {

				//syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath, encoder, accImageBuffer));
				syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath));
				syncBuffer.insert(std::make_unique<ImageConfig>(localCounter, kFilePath));

				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				// test
				std::cerr << "Local Counter " << localCounter << '\n';
				//if (localCounter == 10) {
				//	std::ofstream outStream("Hi.txt", std::ifstream::binary);

				//	std::ostream_iterator<char> oi(outStream);
				//	std::copy(accImageBuffer.begin(), accImageBuffer.end(), oi);
				//	std::cerr << "Output" << '\n';

				//}


			} catch (const std::exception &fail) {
				std::cerr << fail.what() << '\n';
			}


			++localCounter;
		}

	}

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


		for (size_t i = 0; i < kNumOfCamera; ++i) {

			// KAOCC: TODO: add camera config
			auto* cameraPtr = new PerspectiveCamera(kCameraPos + RadeonRays::float3(kStep * i, 0, 0), kCameraAt + RadeonRays::float3(kStep * i, 0, 0), kCameraUp);
			sceneDataPtr->attachCamera(cameraPtr);

			// Adjust sensor size based on current aspect ratio
			float aspect = (float)kWindowWidth / kWindowHeight;
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


		// Set Output
		renderOutputData.resize(renderFarm.size());
		for (size_t i = 0; i < renderFarm.size(); ++i) {
			renderOutputData[i] = renderFarm[i]->createOutput(kWindowWidth, kWindowHeight);
			renderFarm[i]->setOutput(renderOutputData[i]);
		}

	}

	// helper function for rendering
	void RenderingManager::renderingWorker(size_t configIdx) {

		std::cerr << "Worker " << configIdx << " starts !\n";

		// test
		ImageConfig img;

		int counter = 0;

		while (true) {


			renderFarm[configIdx]->render(*sceneDataPtr, configIdx);


			// test code
			if (counter % 2 == 0) {

				//std::cerr << "----------------- Convert and store ! \n";


				img.setId(configIdx);
				convertOutputToImage(img, configIdx);
				//img.storeToPPM(counter);

				// test
				imageLightField.setSubLightFieldImageWithIndex(configIdx, 0, img);
				imageLightField.setSubLightFieldImageWithIndex(configIdx, 1, img);
				// refresh
				imageLightField.setSubLightFieldRefreshState(configIdx, true);
			}

			++counter;

		}

	}


	// testing only
	void RenderingManager::convertOutputToImage(ImageConfig & img, size_t outputIdx) {

		const size_t kStride = 3;

		std::vector<RadeonRays::float3> fdata(kWindowWidth * kWindowHeight);
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


