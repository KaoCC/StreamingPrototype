#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>

#include "math/mathutils.h"

namespace SP {


	RenderingManager::RenderingManager(SyncBuffer<ImageConfig>& buff) : syncBuffer(buff) {
		//, encoder(CreateEncoder(512, 512)) {

		// allocate renderer
		renderer = std::make_unique<PtRenderer>(1, 5);		// idx, num_of_bounce

		initData();
	}

	RenderingManager::~RenderingManager() {
		if (renderThread) {
			renderThread->join();
		}

		//delete encoder;


		// delete output
		renderer->deleteOutput(renderOutputData);
	}

	void RenderingManager::startRenderThread() {

		// yet to be done

		std::cerr << "Start Render Thread" << std::endl;

		// testing 
		//renderThread = std::make_unique<std::thread>(std::thread(&RenderingManager::testOutput, this, 0));

		renderThread = std::make_unique<std::thread>(std::thread(&RenderingManager::renderingWorker, this));
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



		// KAOCC: TODO: add camera config
		camera.reset(new PerspectiveCamera(kCameraPos, kCameraAt, kCameraUp));
		sceneDataPtr->SetCamera(camera.get());

		// Adjust sensor size based on current aspect ratio
		float aspect = (float)kWindowWidth / kWindowHeight;
		g_camera_sensor_size.y = g_camera_sensor_size.x / aspect;

		camera->setSensorSize(g_camera_sensor_size);
		camera->setDepthRange(g_camera_zcap);
		camera->setFocalLength(g_camera_focal_length);
		camera->setFocusDistance(g_camera_focus_distance);
		camera->setAperture(g_camera_aperture);

		std::cout << "Camera type: " << (camera->getAperture() > 0.f ? "Physical" : "Pinhole") << "\n";			// This might cause problems
		std::cout << "Lens focal length: " << camera->getFocalLength() * 1000.f << "mm\n";
		std::cout << "Lens focus distance: " << camera->getFocusDistance() << "m\n";
		std::cout << "F-Stop: " << 1.f / (camera->getAperture() * 10.f) << "\n";
		std::cout << "Sensor size: " << g_camera_sensor_size.x * 1000.f << "x" << g_camera_sensor_size.y * 1000.f << "mm\n";


		// Set Output
		renderOutputData = renderer->createOutput(kWindowWidth, kWindowHeight);
		renderer->setOutput(renderOutputData);

	}

	// helper function for rendering
	void RenderingManager::renderingWorker() {

		// test
		ImageConfig img;

		while (true) {
			renderer->render(*sceneDataPtr);

			convertOutputToImage(img);
		}

	}


	// testing only
	void RenderingManager::convertOutputToImage(ImageConfig & img) {

		std::vector<RadeonRays::float3> fdata(kWindowWidth * kWindowHeight);
		renderOutputData->getData(fdata.data());

		ImageConfig::ImageBuffer& imgBufferRef = img.getImageData();

		imgBufferRef.resize(fdata.size() * 4);

		// tmp gamma
		float gamma = 2.2f;

		for (size_t i = 0; i < fdata.size(); ++i) {
			imgBufferRef[4 * i] = (unsigned char)RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].x / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
			imgBufferRef[4 * i + 1] = (unsigned char)RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].y / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
			imgBufferRef[4 * i + 2] = (unsigned char)RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].z / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
			imgBufferRef[4 * i + 3] = 1;
		}


	}
}


