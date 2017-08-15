#include "ApiEngine.hpp"

#include <iostream>

namespace SP {


	ApiEngine::ApiEngine(ScreenConfig screenCfg, const std::vector<int>& apiIndexList) {

		std::cerr << "Number of APIs: " << apiIndexList.size() << std::endl;
		std::cerr << "Create APIs" << std::endl;

		for (int index : apiIndexList) {

			auto api = RadeonRays::IntersectionApi::Create(index);

			// make these settings configurable
			api->SetOption("acc.type", "qbvh");
			api->SetOption("bvh.builder", "sah");

			mBackends.push_back(BackendRecord(api, screenCfg));
		}


		// Threads ?
		std::cerr << "Create TnI threads" << std::endl;
		mWorkerThreads.reserve(apiIndexList.size());
		for (auto& backend : mBackends) {
			mWorkerThreads.push_back(std::thread(&ApiEngine::runLoop, this, backend.api, backend.buffer));
		}

		mThreadCount = mWorkerThreads.size();

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

		//KAOCC: this is wrong !!! check pop wait !
		// thread cleanup
		std::for_each(mWorkerThreads.begin(), mWorkerThreads.end(), std::mem_fun_ref(&std::thread::join));
	}

	void ApiEngine::pause() {

		std::cerr << "Enter API Pause" << std::endl;

		{
			std::unique_lock<std::mutex> queueLock(mQueueMutex);
			mPauseFlag = true;

			// wait here until all threads are paused

			mCounterCV.wait(queueLock, [this] { return mWaitingCounter == mThreadCount; });
		}


		std::cerr << "Finish API Pause" << std::endl;
	}

	void ApiEngine::resume() {

		{
			std::lock_guard<std::mutex> lock(mQueueMutex);
			mPauseFlag = false;
		}

		mQueueCV.notify_all();

	}

	void ApiEngine::clear() {

		std::lock_guard<std::mutex> lock(mQueueMutex);
		std::queue<std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)>>().swap(mTaskQueue);

	}

	std::future<void>
	ApiEngine::queryIntersection(std::vector<RadeonRays::ray>& rayBuffer, int numOfRays, std::vector<RadeonRays::Intersection>& intersectBuffer) {

		std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)> task {
				std::bind(&ApiEngine::intersect, std::placeholders::_1, std::placeholders::_2, IntersectData(rayBuffer, numOfRays, intersectBuffer)) };

		auto taskFuture = task.get_future();

		// enqueue
		{
			std::lock_guard<std::mutex> queueLock(mQueueMutex);
			mTaskQueue.push(std::move(task));
		}

		mQueueCV.notify_one();

		return taskFuture;
	}

	// KAOCC: make it a template
	std::future<void> ApiEngine::queryOcclusion(std::vector<RadeonRays::ray>& shadowrayBuffer, int numOfRays, std::vector<int>& shadowhitBuffer) {

		std::packaged_task<void(RadeonRays::IntersectionApi* api, BackendBuffer buffer)> task {
				std::bind(&ApiEngine::occlude, std::placeholders::_1, std::placeholders::_2, OccludeData(shadowrayBuffer, numOfRays, shadowhitBuffer)) };

		auto taskFuture = task.get_future();

		// enqueue
		{
			std::lock_guard<std::mutex> queueLock(mQueueMutex);
			mTaskQueue.push(std::move(task));
		}

		mQueueCV.notify_one();

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
			backend.buffer.rays = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			//backend.buffer.rays[1] = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			backend.buffer.shadowrays = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
			backend.buffer.shadowhits = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
			backend.buffer.intersections = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::Intersection), nullptr);
			backend.buffer.hits = backend.api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
			backend.buffer.hitcount = backend.api->CreateBuffer(sizeof(int), nullptr);
		}
	}

	void ApiEngine::compileScene(const Scene& scene) {

		for (auto& backend : mBackends) {
			backend.tracker.compileSceneTest(scene);
		}

	}


	// workaround !
	const std::vector<const Mesh*>& ApiEngine::getInternalMeshPtrs() const {
		return mBackends[0].tracker.getInternalMeshPtrs();
	}

	// workaround !
	const Scene* ApiEngine::getCurrentScenePtr() const {
		return mBackends[0].tracker.getCurrentScenePtr();
	}

	// test
	void ApiEngine::changeShape_test(float x, float y) {
		for (auto& backend : mBackends) {
			//backend.tracker.removeShapesInScene_test();

			// change to add shape
			backend.tracker.addShapesInScene_test(0, 0);

		}
	}


	// for worker thread
	void ApiEngine::runLoop(RadeonRays::IntersectionApi* api, BackendBuffer buffer) {


		std::packaged_task<void(RadeonRays::IntersectionApi*, BackendBuffer)> runFunction;
		while (true) {

			{
				std::unique_lock<std::mutex> queueLock(mQueueMutex);

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


				runFunction = std::move(mTaskQueue.front());
				mTaskQueue.pop();

			}


			//async exec
			runFunction(api, buffer);

		}
	}


	void ApiEngine::intersect(RadeonRays::IntersectionApi* api, BackendBuffer buffer, IntersectData data) {

		// copy host data to buffers

		RadeonRays::Event* writeEvent = nullptr;
		RadeonRays::ray* rawRayPtr = nullptr;

		// memory map
		api->MapBuffer(buffer.rays, RadeonRays::kMapWrite, 0, data.numOfRays * sizeof(RadeonRays::ray),
					   reinterpret_cast<void**>(&rawRayPtr), &writeEvent);
		writeEvent->Wait();
		api->DeleteEvent(writeEvent);
		writeEvent = nullptr;

		// to RR memory
		std::copy_n(data.rayBuffer.begin(), data.numOfRays, rawRayPtr);

		api->UnmapBuffer(buffer.rays, static_cast<void*>(rawRayPtr), &writeEvent);
		writeEvent->Wait();
		api->DeleteEvent(writeEvent);


		// T and I
		api->QueryIntersection(buffer.rays, data.numOfRays, buffer.intersections, nullptr, nullptr);

		// copy buffer data back to host

		RadeonRays::Event* mapEvent = nullptr;
		RadeonRays::Intersection* resultPtr = nullptr;

		api->MapBuffer(buffer.intersections, RadeonRays::kMapRead, 0, data.numOfRays * sizeof(RadeonRays::Intersection),
					   reinterpret_cast<void**>(&resultPtr), &mapEvent);
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

	void ApiEngine::occlude(RadeonRays::IntersectionApi* api, BackendBuffer buffer, OccludeData data) {

		RadeonRays::Event* mapEvent = nullptr;
		RadeonRays::ray* rawShadowrayPtr = nullptr;

		// memory map
		api->MapBuffer(buffer.shadowrays, RadeonRays::kMapWrite, 0, data.numOfRays * sizeof(RadeonRays::ray), reinterpret_cast<void**>(&rawShadowrayPtr),
					   &mapEvent);
		mapEvent->Wait();
		api->DeleteEvent(mapEvent);
		mapEvent = nullptr;

		// to RR memory
		std::copy_n(data.shadowrayBuffer.begin(), data.numOfRays, rawShadowrayPtr);

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


	ApiEngine::BackendRecord::BackendRecord(RadeonRays::IntersectionApi* inputApi, ScreenConfig screenCfg) :
			api { inputApi }, tracker { inputApi } {

		// disable this part ?
		buffer.rays = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		//buffer.rays[1] = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowrays = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::ray), nullptr);
		buffer.shadowhits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.intersections = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(RadeonRays::Intersection), nullptr);
		buffer.hits = api->CreateBuffer(screenCfg.width * screenCfg.height * sizeof(int), nullptr);
		buffer.hitcount = api->CreateBuffer(sizeof(int), nullptr);

	}


	ApiEngine::IntersectData::IntersectData(std::vector<RadeonRays::ray>& rayB, int nR, std::vector<RadeonRays::Intersection>& interB) :
			rayBuffer { rayB }, numOfRays { nR }, intersectBuffer { interB } {
	}

	ApiEngine::OccludeData::OccludeData(std::vector<RadeonRays::ray>& shadowrayB, int nR, std::vector<int>& shadowhitB) :
			shadowrayBuffer { shadowrayB }, numOfRays { nR }, shadowhitBuffer { shadowhitB } {
	}

}