#include "ApiEngine.hpp"

#include <iostream>

namespace SP {



	ApiEngine::ApiEngine(ScreenConfig screenCfg, const std::vector<int>& apiIndexList) {

		std::cerr << "Number of APIs: " << apiIndexList.size() << std::endl;
		std::cerr << "Create APIs" << std::endl;
		for (int index : apiIndexList) {
		
			auto api = RadeonRays::IntersectionApi::Create(index);
			api->SetOption("acc.type", "qbvh");
			api->SetOption("bvh.builder", "sah");

			//mApiList.push_back(api);
			//mTrackers.push_back(SceneTracker(api));


			mBackends.push_back(BackendRecord(api, screenCfg));
		}


		// Threads ?
		std::cerr << "Create TnI threads" << std::endl;
		for (auto& backend : mBackends) {
			mWorkerThreads.push_back(std::thread(&ApiEngine::runLoop, this, backend.api, backend.buffer));
		}

		// Buffers ?
		//createBuffers(screenCfg);
	}

	ApiEngine::~ApiEngine() {

		for (auto& backend : mBackends) {
			backend.api->DeleteBuffer(backend.buffer.rays[0]);
			backend.api->DeleteBuffer(backend.buffer.rays[1]);
			backend.api->DeleteBuffer(backend.buffer.shadowrays);
			backend.api->DeleteBuffer(backend.buffer.shadowhits);
			backend.api->DeleteBuffer(backend.buffer.hits);
			backend.api->DeleteBuffer(backend.buffer.intersections);
			backend.api->DeleteBuffer(backend.buffer.hitcount);
		}

		// thread cleanup
		std::for_each(mWorkerThreads.begin(), mWorkerThreads.end(), std::mem_fun_ref(&std::thread::join));
	}

	std::future<void> ApiEngine::queryIntersection(std::vector<RadeonRays::ray>& rayBuffer, int numOfRays, std::vector<RadeonRays::Intersection>& intersectBuffer) {

		std::packaged_task<void(RadeonRays::IntersectionApi * api, BackendBuffer buffer)> task {std::bind(&ApiEngine::intersect, std::placeholders::_1, std::placeholders::_2, IntersectData(rayBuffer, numOfRays, intersectBuffer)) };
	
		auto taskFuture = task.get_future();

		// enqueue
		taskQueue.push(std::move(task));

		return taskFuture;
	}

	void ApiEngine::resizeBuffers(ScreenConfig screenCfg) {
		for (auto& backend : mBackends) {

			// delete old buffers
			backend.api->DeleteBuffer(backend.buffer.rays[0]);
			backend.api->DeleteBuffer(backend.buffer.rays[1]);
			backend.api->DeleteBuffer(backend.buffer.shadowrays);
			backend.api->DeleteBuffer(backend.buffer.shadowhits);
			backend.api->DeleteBuffer(backend.buffer.hits);
			backend.api->DeleteBuffer(backend.buffer.intersections);
			backend.api->DeleteBuffer(backend.buffer.hitcount);

			// create new buffers
			backend.buffer.rays[0] = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			backend.buffer.rays[1] = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			backend.buffer.shadowrays = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			backend.buffer.shadowhits = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
			backend.buffer.intersections = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::Intersection), nullptr);
			backend.buffer.hits = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
			backend.buffer.hitcount = backend.api->CreateBuffer(sizeof(int), nullptr);
		}
	}

	void ApiEngine::compileScene(const Scene & scene) {

		for (auto& backend : mBackends) {
			backend.tracker.compileSceneTest(scene);
		}

	}



	// for worker thread
	void ApiEngine::runLoop(RadeonRays::IntersectionApi * api, BackendBuffer buffer) {

		// yet to be done
		std::packaged_task<void(RadeonRays::IntersectionApi * api, BackendBuffer buffer)> runFunction;
		while (true) {

			if (taskQueue.pop(runFunction)) {

				runFunction(api, buffer);

			} else {
				// TMP !!!
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}


		}
	}


	void ApiEngine::intersect(RadeonRays::IntersectionApi * api, BackendBuffer buffer, IntersectData data) {



	}




	ApiEngine::BackendRecord::BackendRecord(RadeonRays::IntersectionApi * inputApi, ScreenConfig screenCfg) : api(inputApi), tracker(inputApi){

		// disable this part ?
		buffer.rays[0] = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.rays[1] = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowrays = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowhits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.intersections = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::Intersection), nullptr);
		buffer.hits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.hitcount = api->CreateBuffer(sizeof(int), nullptr);

	}

	ApiEngine::IntersectData::IntersectData(std::vector<RadeonRays::ray>& rayB, int nR, std::vector<RadeonRays::Intersection>& interB) : rayBuffer(rayB), numOfRays(nR), intersectBuffer(interB){
	}

}