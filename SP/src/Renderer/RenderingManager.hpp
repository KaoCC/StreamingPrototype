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

#include "../Encoder/Encoder.hpp"

#include "Output.hpp"

#include "../ConfigManager.hpp"

#include "../HeterogeneousQueue/HQ/src/ThreadSafeQueue.hpp"

namespace SP {



	class RenderingManager {

	public:

		RenderingManager() = delete;


		RenderingManager(ConfigManager& cfgRef, bool loadRadianceFlag);

		~RenderingManager();

		void startRenderThread();

	private:


		// for testing
		void testOutput(int id);

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

		RadeonRays::float2 g_camera_sensor_size = RadeonRays::float2(0.036f, 0.024f);  // default full frame sensor 36x24 mm
		RadeonRays::float2 g_camera_zcap = RadeonRays::float2(0.0f, 100000.f);
		float g_camera_focal_length = 0.035f; // 35mm lens
		float g_camera_focus_distance = 1.f;
		float g_camera_aperture = 0.f;


		const int kNumberOfBounce = 5;


		// tmp
		const std::string defaultPath = "../Resources/CornellBox";
		const std::string defaultModelname = "mid-box.objm";

		//tmp
		const float kStep = 0.025f * 2;


		// Scene Data
		std::unique_ptr<Scene> sceneDataPtr;


		// configs
		ConfigManager& mConfigRef;


		// thread
		std::vector<std::thread> renderThreads;

		// Path-Tracing Renderer
		std::vector<std::unique_ptr<PtRenderer>> renderFarm;

		//Encoder* encoder;
		//ImageConfig::ImageBuffer accImageBuffer; // test

		std::vector<Output*> renderOutputData;



		// test
		//std::queue<std::pair<int, int>> mTaskQueue;

		HQ::ThreadSafeQueue<std::pair<int, int>> mTaskQueue;

	};


}





#endif



