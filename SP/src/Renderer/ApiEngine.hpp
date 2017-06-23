#ifndef _SP_APIENGINE_HPP_
#define _SP_APIENGINE_HPP_


#include "SceneTracker.hpp"
#include "../ConfigManager.hpp"

#include <vector>
#include <thread>

namespace SP {

	class ApiEngine {


	public:

		ApiEngine() = delete;

		ApiEngine(ScreenConfig screenCfg, const std::vector<int>& apiIndexList);

		~ApiEngine();


		//void queryIntersection();

		void resizeBuffers(ScreenConfig screenCfg);


		void compileScene(const Scene& scene);

	private:

		// Buffers
		struct BackendBuffer {
			RadeonRays::Buffer* rays[2] = { nullptr, nullptr };
			RadeonRays::Buffer* shadowrays = nullptr;
			RadeonRays::Buffer* shadowhits = nullptr;
			RadeonRays::Buffer* hits = nullptr;
			RadeonRays::Buffer* intersections = nullptr;
			RadeonRays::Buffer* hitcount = nullptr;	
		};


		struct BackendRecord {
		public:
			BackendRecord(RadeonRays::IntersectionApi* api, ScreenConfig screenCfg);

			BackendBuffer buffer;
			RadeonRays::IntersectionApi* api;
			SceneTracker tracker;
		};

		// Buffer management
		//void createBuffers(ScreenConfig screenCfg);

		// API index in parameter ?
		void runLoop(RadeonRays::IntersectionApi* api);

	
		//std::vector<SceneTracker> mTrackers;
		// Intersection APIs
		//std::vector<RadeonRays::IntersectionApi*> mApiList;
		//std::vector<BackendBuffer> mBuffers;

		// no need ?
		std::vector<int> mApiIndex;

		std::vector<std::thread> mWorkerThreads;

		// thread safe queue ?



		std::vector<BackendRecord> mBackends;

	};

}

















#endif



