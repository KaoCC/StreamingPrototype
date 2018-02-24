#include "RenderingManager.hpp"

#include "Scene/IO/SceneIO.hpp"


//for testing
#include <iostream>
#include <chrono>

#include "math/mathutils.h"


#include "SimpleRenderer.hpp"

#include "OpenImageIO/imageio.h"


#include "Scene/Material.hpp"

#include "DefaultList.hpp"

namespace SP {


	// for testing
	static std::unique_ptr<Mesh> createTransformedMesh(float worldX, float worldY, float worldZ, DefaultShapeType type) {

		auto defaultMesh = createDefaultShape(type);

		// Transform !!!
		const RadeonRays::matrix& transMat = RadeonRays::translation({worldX, worldY, worldZ});
		defaultMesh->transform(transMat);


		return defaultMesh;
	}


	// tmp, for testing only
	// legacy code, not use anymore
	[[deprecated("testing code")]]
	static SP::Mesh* createDefaultMesh(float worldX, float worldY, float worldZ) {

		// TODO: fix this !! Who's gonna release the memory for this ?
		SP::Mesh* mesh = new Mesh();

		// original world space
		//float vertices[] {
		//	0.f, 0.f, 0.f,
		//	0.f, 1.f, 0.f,
		//	1.f, 0.f, 0.f
		//};

		const int unitLength = 1;

		float vertices[] {
			worldX, worldY, worldZ,
			worldX, worldY + unitLength, worldZ,
			worldX, worldY, worldZ + unitLength
		};

		size_t numOfVertices = 3;


		uint32_t indices[] { 0, 1, 2 };
		size_t numOfIndices = 3;

		float normals[] {
			1, 0, 0,
			1, 0, 0,
			1, 0, 0
		};
		size_t numOfNormals = 3;


		// set vertices
		mesh->setVertices(vertices, numOfVertices);

		// set indices
		mesh->setIndices(indices, numOfIndices);

		// set Normals
		mesh->setNormals(normals, numOfNormals);

		// Generate Zeros if we do not have UVs
		std::vector<RadeonRays::float2> zero(numOfVertices);
		std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
		mesh->setUVs(&zero[0], numOfVertices);

		mesh->setName("Default");

		// memory leak !
		std::unique_ptr<Material> diffuse = std::make_unique<SingleBxDF>(SingleBxDF::BxDFType::kLambert);
		diffuse->setInputValue("albedo", RadeonRays::float3(10, 10, 10));

		mesh->setMaterial(std::move(diffuse));

		return mesh;
	}


	RenderingManager::RenderingManager(ConfigManager& cfgRef, bool loadRadianceFlag) : mConfigRef { cfgRef } {


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
			//std::printf("DeviceInfo: [%s] [%s] [%i] [%x]\n", devinfo.name, devinfo.vendor, devinfo.type, devinfo.platform);

			std::cerr << "Device Info:" << " [" << devinfo.name << "][" << devinfo.vendor << "][" << devinfo.type << "][" << devinfo.platform << "]"
					  << std::endl;

			if (devinfo.type == RadeonRays::DeviceInfo::kCpu && cpuIdx == -1) {
				cpuIdx = idx;
			}

			if (devinfo.type == RadeonRays::DeviceInfo::kGpu && gpuIdx == -1) {
				gpuIdx = idx;
			}

			if (devinfo.platform == RadeonRays::DeviceInfo::kEmbree && devinfo.type == RadeonRays::DeviceInfo::kCpu && embreeIdx == -1) {
				embreeIdx = idx;
			}
		}

		std::vector<int> apiIndex;

		//int nativeIdx = -1;

		// select order:  GPU > Embree > CPU
		if (gpuIdx != -1) {
			//nativeIdx = gpuIdx;
			apiIndex.push_back(gpuIdx);

		}

		if (embreeIdx != -1) {
			//nativeIdx = embreeIdx;
			apiIndex.push_back(embreeIdx);
		}

		if (cpuIdx != -1) {
			//nativeIdx = cpuIdx;
			apiIndex.push_back(cpuIdx);
		}

		// KAOCC: we should support multiple GPUs !

		//std::cerr << "Selected Device ID: " << nativeIdx << std::endl;

		ScreenConfig screenCfg { mConfigRef.getScreenWidth(), mConfigRef.getScreenHeight() };


		// API Engine
		mEnginePtr = std::make_unique<ApiEngine>(screenCfg, apiIndex);


		// Scene Tracker
		mTracker = std::make_unique<SceneTracker>(mEnginePtr->getAPIs());

		// change this
		// allocate renderer
		renderFarm.resize(cfgRef.getNumberOfCameras());
		for (auto& renderItem : renderFarm) {
			renderItem = std::make_unique<PtRenderer>(kNumberOfBounce, *mEnginePtr);        // num_of_bounce
			//renderFarm[i] = std::make_unique<SimpleRenderer>(mEnginePtr);
		}

		//renderThreads.resize(renderFarm.size());

		initData(loadRadianceFlag);
	}

	RenderingManager::~RenderingManager() {


		// KAOCC: this is wrong ! check pop wait 
		std::for_each(renderThreads.begin(), renderThreads.end(), std::mem_fn(&std::thread::join));


		// delete output
		//for (size_t i = 0; i < renderFarm.size(); ++i) {
		//	renderFarm[i]->deleteOutput(renderOutputData[i]);
		//}
	}

	void RenderingManager::startRenderThread() {


		std::cerr << "Start Render Thread" << std::endl;


		{
			std::lock_guard<std::mutex> queueLock { mQueueMutex };

			for (size_t i = 0; i < mConfigRef.getNumberOfSubLFs(); ++i) {
				for (size_t j = 0; j < mConfigRef.getNumberOfSubLFImages(); ++j) {
					mTaskQueue.push(std::make_pair(i, j));
				}
			}
		}

		unsigned int numOfThreads = std::thread::hardware_concurrency();

		if (numOfThreads == 0) {
			numOfThreads = 4;
		}

		std::cout << ">>> number of threads: " << numOfThreads << std::endl;

		// set thread count

		mThreadCount = numOfThreads;


		for (size_t i = 0; i < numOfThreads; ++i) {
			renderThreads.push_back(std::thread(&RenderingManager::renderingWorker, this));
		}

	}

	void RenderingManager::pause() {

		std::cerr << "Enter Pause" << std::endl;

		{
			std::unique_lock<std::mutex> queueLock { mQueueMutex };
			mPauseFlag = true;

			mCounterCV.wait(queueLock, [this] { return mWaitingCounter == mThreadCount; });
		}

		std::cerr << "Finish Pause" << std::endl;

	}

	void RenderingManager::resume() {

		{
			std::lock_guard<std::mutex> lock { mQueueMutex };
			mPauseFlag = false;
		}

		mQueueCV.notify_all();
	}

	void RenderingManager::reset(ConfigManager::State state) {

		// pause first
		pause();

		// clear queue ?

		// pause, clear and reset API Engine ?
		mEnginePtr->pause();
		mEnginePtr->clear();

		// reset Scene ?

		std::cerr << "Resetting ... " << std::endl;

		if (state == ConfigManager::State::kSimple) {

			for (auto& renderItem : renderFarm) {
				renderItem = std::make_unique<SimpleRenderer>(*mEnginePtr);
			}

		} else if (state == ConfigManager::State::kPathTracing) {
			for (auto& renderItem : renderFarm) {
				renderItem = std::make_unique<PtRenderer>(kNumberOfBounce, *mEnginePtr);
			}

		} else {
			throw std::runtime_error("Unsupported state");
		}


		for (size_t i = 0; i < renderFarm.size(); ++i) {
			//renderOutputData[i] = renderFarm[i]->createOutput(mConfigRef.getScreenWidth(), mConfigRef.getScreenHeight());
			renderFarm[i]->setOutput(renderOutputData[i]);
		}


		std::cerr << "Resumming ..." << std::endl;

		// resume all
		mEnginePtr->resume();
		resume();
	}

	void RenderingManager::changeSceneWithCoordinates(float worldX, float worldY, float worldZ, DefaultShapeType type) {

		pause();

		mEnginePtr->pause();
		mEnginePtr->clear();

		std::cerr << "recompile Pause ... " << std::endl;


		// TODO: check if we need to clear output?
		//for (size_t i = 0; i < renderFarm.size(); ++i) {
		//	renderFarm[i]->clear(0, *renderOutputData[i]);
		//}


		// add new shapes
		// API commit 
		// rebuild BVH

		//mEnginePtr->changeShape_test(worldX, worldY, worldZ);


		//sceneDataPtr->attachShape(createDefaultMesh(worldX, worldY, worldZ));

		//auto* mesh = createTransformedMesh(worldX, worldY, worldZ, type);

		sceneDataPtr->attachShape(createTransformedMesh(worldX, worldY, worldZ, type));

		//mEnginePtr->compileScene(*sceneDataPtr);

		mTracker->compileSceneTest(*sceneDataPtr);
		

		std::cerr << "recompile Resume ..." << std::endl;

		mEnginePtr->resume();
		resume();

	}

	void RenderingManager::removeLastShape() {
		sceneDataPtr->detachLastShape();
	}

	const PerspectiveCamera & RenderingManager::getPerspectiveCamera(size_t index) const {

		return dynamic_cast<const PerspectiveCamera&>(sceneDataPtr->getCamera(index));

	}

	void RenderingManager::createDefaultList() {

		if (defaultShapeList.empty()) {
			// add support type here
			//defaultShapeList.push_back(DefaultShapeType::kTriangle);
			//defaultShapeList.push_back(DefaultShapeType::kSquare);
			//defaultShapeList.push_back(DefaultShapeType::kBudda);

			defaultShapeList = { DefaultShapeType::kTriangle , DefaultShapeType::kSquare , DefaultShapeType::kBudda };
		}

	}

	const std::vector<DefaultShapeType>& RenderingManager::getDefaultList() const {
		return defaultShapeList;
	}


	void RenderingManager::initData(bool loadRadianceFlag) {


		std::cerr << "init data start" << std::endl;


		// rand
		RadeonRays::rand_init();

		// Load obj file
		std::string basepath = defaultPath;
		basepath += "/";
		std::string filename = basepath + defaultModelName;


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
			renderOutputData[i] = std::make_shared<RenderOutput>(mConfigRef.getScreenWidth(), mConfigRef.getScreenHeight());
			renderFarm[i]->setOutput(renderOutputData[i]);
		}


		auto& fieldRef = mConfigRef.getLightField();

		const auto& camDefault = mConfigRef.getCameraConfig();

		for (size_t i = 0; i < mConfigRef.getNumberOfSubLFs(); ++i) {

			for (size_t j = 0; j < mConfigRef.getNumberOfSubLFImages(); ++j) {

				// KAOCC: TODO: add camera config

				// Following is for camera look toward +X
				//auto* cameraPtr = new PerspectiveCamera(camDefault.mCameraPos + RadeonRays::float3(0, kStep * j, -kStep * i),
				//										camDefault.mCameraAt + RadeonRays::float3(0, kStep * j, -kStep * i), camDefault.mCameraUp);

				// Following is for camera look toward -Z
				auto cameraPtr =  std::make_unique<PerspectiveCamera>(camDefault.mCameraPos + RadeonRays::float3(-kStep * i, kStep * j,0),
					camDefault.mCameraAt + RadeonRays::float3(-kStep * i, kStep * j, 0), camDefault.mCameraUp);


				// Adjust sensor size based on current aspect ratio
				float aspect = (float) mConfigRef.getScreenWidth() / mConfigRef.getScreenHeight();
				cameraSensorSize.y = cameraSensorSize.x / aspect;

				cameraPtr->setSensorSize(cameraSensorSize);
				cameraPtr->setDepthRange(cameraZcap);
				cameraPtr->setFocalLength(cameraFocalLength);
				cameraPtr->setFocusDistance(cameraFocusDistance);
				cameraPtr->setAperture(cameraAperture);

				std::cout << "Camera type: " << (cameraPtr->getAperture() > 0.f ? "Physical" : "Pinhole") << "\n";            // This might cause problems
				std::cout << "Lens focal length: " << cameraPtr->getFocalLength() * 1000.f << "mm\n";
				std::cout << "Lens focus distance: " << cameraPtr->getFocusDistance() << "m\n";
				std::cout << "F-Stop: " << 1.f / (cameraPtr->getAperture() * 10.f) << "\n";
				std::cout << "Sensor size: " << cameraSensorSize.x * 1000.f << "x" << cameraSensorSize.y * 1000.f << "mm\n";


				sceneDataPtr->attachCamera(std::move(cameraPtr));

				// test !
				//sceneDataPtr->attachAutoreleaseObject(cameraPtr);


				// Link to RenderOutput
				fieldRef[i][j].setRadiancePtr(renderOutputData[mConfigRef.getNumberOfSubLFImages() * i + j]);

				// load radiamce map if the flag is set

				if (loadRadianceFlag) {
					loadRadianceOutput(i, j);
				}


			}

		}

		mTracker->compileSceneTest(*sceneDataPtr);
		//mEnginePtr->compileScene(*sceneDataPtr);
	}

	void RenderingManager::loadRadianceOutput(int subLFIdx, int subImgIdx) {

		// load Radiance Map to RenderOut

		unsigned outputId = mConfigRef.getNumberOfSubLFImages() * subLFIdx + subImgIdx;

		std::shared_ptr<RenderOutput> renderOut = renderOutputData[outputId];

		if (renderOut == nullptr) {
			throw std::runtime_error("RenderOutput is null");
		}

		auto& outputData = *renderOut;

		OIIO_NAMESPACE_USING


		const std::string& subFix = std::to_string(outputId);

		std::string filename = "radiance" + subFix + '-' + subFix + ".exr";

		std::cerr << "Load file name: " << filename << std::endl;

		ImageInput* input = ImageInput::open(filename);

		if (input == nullptr) {
			throw std::runtime_error("Failed to load image: " + filename);
		}


		const ImageSpec imgSpec = input->spec();
		int xRes = imgSpec.width;
		int yRes = imgSpec.height;

		int channels = imgSpec.nchannels;    // check

		const ParamValue* par { imgSpec.find_attribute("gammarank", TypeDesc::FLOAT) };

		float wVal = 10.0f;        // tmp value
		if (par != nullptr) {
			wVal = *(static_cast<const float*>(par->data()));
		} else {
			std::cerr << "MetaData (gammarank) not found, roll back to the default" << std::endl;
		}

		const int totalPixelNum = xRes * yRes;

		const int tmpSize = totalPixelNum * channels;

		// check this !
		outputData.resize(xRes, yRes);


		// create a tmp buffer 
		float* tmpBuff { new float[tmpSize] };

		// copy to outputData
		input->read_image(TypeDesc::FLOAT, tmpBuff);


		//dump the tmp data to output
		for (int i = 0; i < outputData.getSize(); ++i) {
			outputData[i].x = tmpBuff[i * channels];
			outputData[i].y = tmpBuff[i * channels + 1];
			outputData[i].z = tmpBuff[i * channels + 2];
			outputData[i].w = wVal;
		}

		// close
		input->close();
		delete input;
		delete[] tmpBuff;

	}

	void RenderingManager::renderingWorker() {

		std::pair<int, int> taskIndex;

		while (true) {


			{
				std::unique_lock<std::mutex> queueLock { mQueueMutex };

				if (mTaskQueue.empty() || mPauseFlag) {

					++mWaitingCounter;

					if (mWaitingCounter == mThreadCount) {
						queueLock.unlock();
						mCounterCV.notify_one();
						queueLock.lock();
					}

					mQueueCV.wait(queueLock, [this] { return !(mTaskQueue.empty() || mPauseFlag); });
					--mWaitingCounter;
				}


				taskIndex = std::move(mTaskQueue.front());
				mTaskQueue.pop();

			}


			// async part

			size_t subLFIdx = taskIndex.first;
			size_t subImgIdx = taskIndex.second;

			//std::cerr << "Worker " << subLFIdx << ' ' << subImgIdx << " starts !\n";

			size_t farmIdx = mConfigRef.getNumberOfSubLFImages() * subLFIdx + subImgIdx;

			// rendering
			auto t1 = std::chrono::high_resolution_clock::now();
			renderFarm[farmIdx]->render(*sceneDataPtr, farmIdx);
			auto t2 = std::chrono::high_resolution_clock::now();

			// test
			//if (farmIdx == 0) {
			//	std::cerr << "FarmIndex: " << farmIdx << " Update time: " 
			//		<< std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl;
			//}


			// tmp, need lock , need interrupt-based method
			if (mConfigRef.isSceneChanged(farmIdx)) {
				//renderFarm[farmIdx]->clear(0.f, *(renderOutputData[farmIdx]));

				renderOutputData[farmIdx]->resetToDefault();

				mConfigRef.setSceneChangedFlag(farmIdx, false);
			}

			auto& fieldRef = mConfigRef.getLightField();

			fieldRef[subLFIdx][subImgIdx].setRefreshState(true);

			// for saving images
			fieldRef[subLFIdx][subImgIdx].setId(farmIdx);


			// push back the task
			{
				std::lock_guard<std::mutex> queueLock { mQueueMutex };
				mTaskQueue.push(std::move(taskIndex));

				// TODO : Two tasks with the same ID can not be in the queue at the same time ! Need a fix.

			}
			mQueueCV.notify_one();

		}


	}


}


