#ifndef _SP_APIENGINE_HPP_
#define _SP_APIENGINE_HPP_


#include "SceneTracker.hpp"
#include "../ConfigManager.hpp"

#include <vector>
#include <thread>

#include <queue>
#include <future>

namespace SP {

	class ApiEngine {


	public:

		ApiEngine() = delete;
		ApiEngine(const ApiEngine&) = delete;

		ApiEngine(ScreenConfig screenCfg, const std::vector<int>& apiIndexList);

		~ApiEngine();


		void pause();
		void resume();

		void clear();

		std::future<void> queryIntersection(std::vector<RadeonRays::ray>& rayBuffer, int numOfRays ,std::vector<RadeonRays::Intersection>& intersectBuffer);
		std::future<void> queryOcclusion(std::vector<RadeonRays::ray>& shadowrayBuffer, int numOfRays, std::vector<int>& shadowhitBuffer);

		void resizeBuffers(ScreenConfig screenCfg);


		void compileScene(const Scene& scene);

		//workaround
		const std::vector<const Mesh*>& getInternalMeshPtrs() const;


		//workaround !
		const Scene* getCurrentScenePtr() const;

		// test
		void changeShape_test(float worldX, float worldY, float worldZ);

	private:

		struct IntersectData {
			IntersectData(std::vector<RadeonRays::ray>& rayB, int nR, std::vector<RadeonRays::Intersection>& interB);
			std::vector<RadeonRays::ray>& rayBuffer;
			int numOfRays;
			std::vector<RadeonRays::Intersection>& intersectBuffer;
		};


		struct OccludeData {
			OccludeData(std::vector<RadeonRays::ray>& shadowrayB, int nR, std::vector<int>& shadowhitB);
			std::vector<RadeonRays::ray>& shadowrayBuffer;
			int numOfRays;
			std::vector<int>& shadowhitBuffer;
		};

		// Buffers
		struct BackendBuffer {
			RadeonRays::Buffer* rays = nullptr;
			RadeonRays::Buffer* shadowrays = nullptr;
			RadeonRays::Buffer* shadowhits = nullptr;
			RadeonRays::Buffer* hits = nullptr;
			RadeonRays::Buffer* intersections = nullptr;
			RadeonRays::Buffer* hitcount = nullptr;	
		};


		struct BackendRecord {
		public:
			BackendRecord(RadeonRays::IntersectionApi* api, ScreenConfig screenCfg);
			BackendRecord() = delete;
			//disallow copy
			BackendRecord(const BackendRecord&) = delete;
			BackendRecord& operator=(const BackendRecord&) = delete;
			// allow move
			BackendRecord(BackendRecord&&) = default;
			BackendRecord& operator=(BackendRecord&&) = default;

			BackendBuffer buffer;
			RadeonRays::IntersectionApi* api;
			SceneTracker tracker;
		};

		// Buffer management
		//void createBuffers(ScreenConfig screenCfg);

		// API index in parameter ?
		void runLoop(RadeonRays::IntersectionApi* api, BackendBuffer buffer);


		static void intersect(RadeonRays::IntersectionApi* api, BackendBuffer buffer, IntersectData data);
		static void occlude(RadeonRays::IntersectionApi* api, BackendBuffer buffer, OccludeData data);
	
		//std::vector<SceneTracker> mTrackers;
		// Intersection APIs
		//std::vector<RadeonRays::IntersectionApi*> mApiList;
		//std::vector<BackendBuffer> mBuffers;

		// no need ?
		//std::vector<int> mApiIndex;

		std::vector<std::thread> mWorkerThreads;


		// thread safe queue ?
		//HQ::ThreadSafeQueue<std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)>> taskQueue;


		std::mutex mQueueMutex;
		std::condition_variable mQueueCV;
		std::queue<std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)>> mTaskQueue;

		bool mPauseFlag = false;

		// counter cv
		std::condition_variable mCounterCV;

		unsigned mWaitingCounter = 0;
		unsigned mThreadCount = 0;

		std::vector<BackendRecord> mBackends;



	};

}

















#endif



