#ifndef _SP_RENDERINGMANAGER_HPP_
#define _SP_RENDERINGMANAGER_HPP_

#include "Scene/Camera.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <memory>
#include <string>

#include <queue>

#include "Scene/Scene.hpp"
#include "../SyncBuffer.hpp"
#include "Common.hpp"
#include "PtRenderer.hpp"

//#include "../Encoder/Encoder.hpp"

#include "Output.hpp"

#include "../ConfigManager.hpp"

#include "ApiEngine.hpp"

#include "DefaultList.hpp"

namespace SP {



	class RenderingManager {

	public:


		RenderingManager() = delete;


		RenderingManager(ConfigManager& cfgRef, bool loadRadianceFlag);

		~RenderingManager();

		void startRenderThread();

		void enqueueRenderTask(const RenderingTask& task);
		void enqueueRenderTask(size_t subLFIdx = 0, size_t subImgIdx = 0, bool needRenderDepth = true, int sampleCount = -1);


		void pause();
		void resume();

		// reset the data
		void reset(ConfigManager::State state);


		// KAOCC: tmp testing
		void changeSceneWithCoordinates(float worldX, float worldY, float worldZ, DefaultShapeType type);

		// testing 
		const PerspectiveCamera& getPerspectiveCamera(size_t index) const;


		// tmp
		//void compileScene();

		// tmp, Default list

		void createDefaultList();
		const std::vector<DefaultShapeType>& getDefaultList() const;


	private:

		

		// for testing
		//void testOutput(int id);

		// helper function
		void initData(bool loadRadianceFlag);

		// helper function for radiance loading

		void loadRadianceOutput(int subLFIdx, int subImgIdx);

		// helper function
		//void renderingWorker(size_t subLFIdx, size_t subImgIdx);

		// helper function for worker thread
		void renderingWorker(void);


		// Output convert
		// tmp
		//void convertOutputToImage(ImageConfig& img, size_t outputIdx);


		// Camera
		// KAOCC: support PerspectiveCamera only !
		//std::unique_ptr<SP::PerspectiveCamera> camera;

		// tmp
		// default camera parameters
		//const RadeonRays::float3 kCameraPos = RadeonRays::float3(0.f, 1.f, 3.f);
		//const RadeonRays::float3 kCameraAt = RadeonRays::float3(0.f, 1.f, 0.f);
		//const RadeonRays::float3 kCameraUp = RadeonRays::float3(0.f, 1.f, 0.f);

		RadeonRays::float2 cameraSensorSize = RadeonRays::float2(0.036f, 0.024f);  // default full frame sensor 36x24 mm
		RadeonRays::float2 cameraZcap = RadeonRays::float2(0.0f, 30.f);
		float cameraFocalLength = 0.010f; // 35mm lens
		float cameraFocusDistance = 1.f;
		float cameraAperture = 0.f;


		const unsigned kNumberOfBounce = 5;


		// tmp
		const std::string defaultPath = "Resources/Conf";
		const std::string defaultModelName = "conf_room_6.objm";

		//tmp
		const float kStep = 0.025f * 6;

		// thread safe queue
		const int kPauseTime = 100;

		// Scene Data
		std::unique_ptr<Scene> sceneDataPtr;


		// configs
		ConfigManager& mConfigRef;


		// thread
		std::vector<std::thread> renderThreads;

		// Path-Tracing Renderer
		std::vector<std::unique_ptr<Renderer>> renderFarm;


		//std::unique_ptr<HQ::EventSys> mPauseEventPtr;

		//Encoder* encoder;
		//ImageConfig::ImageBuffer accImageBuffer; // test

		std::vector<std::shared_ptr<RenderOutput>> renderOutputData;

		std::unique_ptr<ApiEngine> mEnginePtr;
		std::unique_ptr<SceneTracker> mTracker;

		// test
		//std::queue<std::pair<int, int>> mTaskQueue;

		//HQ::ThreadSafeQueue<std::pair<int, int>> mTaskQueue;

		std::mutex mQueueMutex;
		std::condition_variable mQueueCV;
		std::queue<RenderingTask> mTaskQueue;

		unsigned mThreadCount = 0;
		unsigned mWaitingCounter = 0;

		std::condition_variable mCounterCV;

		bool mPauseFlag = false;



		// default list
		std::vector<DefaultShapeType> defaultShapeList;

	};


}





#endif



