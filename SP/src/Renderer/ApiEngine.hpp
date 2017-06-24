#ifndef _SP_APIENGINE_HPP_
#define _SP_APIENGINE_HPP_


#include "SceneTracker.hpp"
#include "../ConfigManager.hpp"

#include <vector>
#include <thread>

#include "../HeterogeneousQueue/HQ/src/ThreadSafeQueue.hpp"

namespace SP {

	class ApiEngine {


	public:

		ApiEngine() = delete;

		ApiEngine(ScreenConfig screenCfg, const std::vector<int>& apiIndexList);

		~ApiEngine();


		std::future<void> queryIntersection(std::vector<RadeonRays::ray>& rayBuffer, int numOfRays ,std::vector<RadeonRays::Intersection>& intersectBuffer);

		void resizeBuffers(ScreenConfig screenCfg);


		void compileScene(const Scene& scene);

	private:

		struct IntersectData {
			IntersectData(std::vector<RadeonRays::ray>& rayB, int nR, std::vector<RadeonRays::Intersection>& interB);
			std::vector<RadeonRays::ray>& rayBuffer;
			int numOfRays;
			std::vector<RadeonRays::Intersection>& intersectBuffer;
		};

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
		void runLoop(RadeonRays::IntersectionApi* api, BackendBuffer buffer);


		static void intersect(RadeonRays::IntersectionApi* api, BackendBuffer buffer, IntersectData data);
	
		//std::vector<SceneTracker> mTrackers;
		// Intersection APIs
		//std::vector<RadeonRays::IntersectionApi*> mApiList;
		//std::vector<BackendBuffer> mBuffers;

		// no need ?
		std::vector<int> mApiIndex;

		std::vector<std::thread> mWorkerThreads;


		// thread safe queue ?
		HQ::ThreadSafeQueue<std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)>> taskQueue;


		std::vector<BackendRecord> mBackends;

	};

}

















#endif



