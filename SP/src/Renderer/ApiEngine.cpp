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
			backend.api->DeleteBuffer(backend.buffer.rays);
			//backend.api->DeleteBuffer(backend.buffer.rays[1]);
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

		std::packaged_task<void(RadeonRays::IntersectionApi * api, BackendBuffer buffer)> task{ std::bind(&ApiEngine::intersect, std::placeholders::_1, std::placeholders::_2, IntersectData(rayBuffer, numOfRays, intersectBuffer)) };

		auto taskFuture = task.get_future();

		// enqueue
		taskQueue.push(std::move(task));

		return taskFuture;
	}

	// KAOCC: make it a template
	std::future<void> ApiEngine::queryOcclusion(std::vector<RadeonRays::ray>& shadowrayBuffer, int numOfRays, std::vector<int>& shadowhitBuffer) {

		std::packaged_task<void(RadeonRays::IntersectionApi * api, BackendBuffer buffer)> task{ std::bind(&ApiEngine::occlude, std::placeholders::_1, std::placeholders::_2, OccludeData(shadowrayBuffer, numOfRays, shadowhitBuffer)) };

		auto taskFuture = task.get_future();

		// enqueue
		taskQueue.push(std::move(task));

		return taskFuture;

	}

	void ApiEngine::resizeBuffers(ScreenConfig screenCfg) {
		for (auto& backend : mBackends) {

			// delete old buffers
			backend.api->DeleteBuffer(backend.buffer.rays);
			//backend.api->DeleteBuffer(backend.buffer.rays[1]);
			backend.api->DeleteBuffer(backend.buffer.shadowrays);
			backend.api->DeleteBuffer(backend.buffer.shadowhits);
			backend.api->DeleteBuffer(backend.buffer.hits);
			backend.api->DeleteBuffer(backend.buffer.intersections);
			backend.api->DeleteBuffer(backend.buffer.hitcount);

			// create new buffers
			backend.buffer.rays= backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			//backend.buffer.rays[1] = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
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


	// workaround !
	const std::vector<const Mesh*>& ApiEngine::getInternalMeshPtrs() const {
		return mBackends[0].tracker.getInternalMeshPtrs();
	}

	// workaround !
	const Scene * ApiEngine::getCurrentScenePtr() const {
		return mBackends[0].tracker.getCurrentScenePtr();
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
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}


		}
	}


	void ApiEngine::intersect(RadeonRays::IntersectionApi * api, BackendBuffer buffer, IntersectData data) {

		// copy host data to buffers

		RadeonRays::Event* writeEvent = nullptr;
		RadeonRays::ray* rawRayPtr = nullptr;

		// memory map
		api->MapBuffer(buffer.rays, RadeonRays::kMapWrite, 0, data.numOfRays * sizeof(RadeonRays::ray), reinterpret_cast<void**>(&rawRayPtr), &writeEvent);
		writeEvent->Wait();
		api->DeleteEvent(writeEvent);
		writeEvent = nullptr;

		// to RR memory
		std::copy(data.rayBuffer.begin(), data.rayBuffer.end(), rawRayPtr);

		api->UnmapBuffer(buffer.rays, static_cast<void*>(rawRayPtr), &writeEvent);
		writeEvent->Wait();
		api->DeleteEvent(writeEvent);


		// T and I
		api->QueryIntersection(buffer.rays, data.numOfRays, buffer.intersections, nullptr, nullptr);

		// copy buffer data back to host

		RadeonRays::Event* mapEvent = nullptr;
		RadeonRays::Intersection* resultPtr = nullptr;

		api->MapBuffer(buffer.intersections, RadeonRays::kMapRead, 0, data.numOfRays * sizeof(RadeonRays::Intersection), reinterpret_cast<void**>(&resultPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);

		// copy intersections
		std::copy(resultPtr, resultPtr + data.numOfRays, data.intersectBuffer.begin());


		// unmap
		mapEvent = nullptr;
		api->UnmapBuffer(buffer.intersections, static_cast<void*>(resultPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);

	}

	void ApiEngine::occlude(RadeonRays::IntersectionApi * api, BackendBuffer buffer, OccludeData data) {

		RadeonRays::Event* mapEvent = nullptr;
		RadeonRays::ray* rawShadowrayPtr = nullptr;

		// memory map
		api->MapBuffer(buffer.shadowrays, RadeonRays::kMapWrite, 0, data.numOfRays * sizeof(RadeonRays::ray), reinterpret_cast<void**>(&rawShadowrayPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);
		mapEvent = nullptr;

		// to RR memory
		std::copy(data.shadowrayBuffer.begin(), data.shadowrayBuffer.end(), rawShadowrayPtr);

		api->UnmapBuffer(buffer.shadowrays, static_cast<void*>(rawShadowrayPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);

		// occlude
		api->QueryOcclusion(buffer.shadowrays, data.numOfRays, buffer.shadowhits, nullptr, nullptr);

		// copy back 

		int* shadowResultPtr = nullptr;

		mapEvent = nullptr;
		api->MapBuffer(buffer.shadowhits, RadeonRays::kMapRead, 0, data.numOfRays * sizeof(int), reinterpret_cast<void**>(&shadowResultPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);


		// copy shadowhits
		std::copy(shadowResultPtr, shadowResultPtr + data.numOfRays, data.shadowhitBuffer.begin());


		mapEvent = nullptr;
		api->UnmapBuffer(buffer.shadowhits, static_cast<void*>(shadowResultPtr), &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);

	}




	ApiEngine::BackendRecord::BackendRecord(RadeonRays::IntersectionApi * inputApi, ScreenConfig screenCfg) : api(inputApi), tracker(inputApi) {

		// disable this part ?
		buffer.rays = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		//buffer.rays[1] = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowrays = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowhits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.intersections = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::Intersection), nullptr);
		buffer.hits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.hitcount = api->CreateBuffer(sizeof(int), nullptr);

	}

	ApiEngine::IntersectData::IntersectData(std::vector<RadeonRays::ray>& rayB, int nR, std::vector<RadeonRays::Intersection>& interB) : rayBuffer(rayB), numOfRays(nR), intersectBuffer(interB) {
	}

	ApiEngine::OccludeData::OccludeData(std::vector<RadeonRays::ray>& shadowrayB, int nR, std::vector<int>& shadowhitB) : shadowrayBuffer(shadowrayB), numOfRays(nR), shadowhitBuffer(shadowhitB) {
	}

}