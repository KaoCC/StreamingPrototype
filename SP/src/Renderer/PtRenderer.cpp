#include "PtRenderer.hpp"
#include "Sampler.hpp"

#include "Scene/Camera.hpp"

#include "Path.hpp"

#include <iostream>
#include <numeric>

namespace SP {


	//struct PtRenderer::PathState {
	//	RadeonRays::float4 throughput;
	//	int volume;
	//	int flags;
	//	int extra_0;
	//	int extra_1;
	//};


	struct PtRenderer::RenderData {


		// host buffers

		std::vector<RadeonRays::ray> host_rays[2];
		std::vector<int> host_hits;

		std::vector<int> host_pixelIndex[2];
		std::vector<int> host_compactedIndex;
		std::vector<int> host_iota;

		std::vector<RadeonRays::ray> host_shadowrays;
		std::vector<int> host_shadowhits;

		std::vector<RadeonRays::Intersection> host_intersections;
		int host_hitcount;


		std::vector<Path> host_path;


		// RadeonRays stuff
		RadeonRays::Buffer* rays[2] = {nullptr, nullptr};
		RadeonRays::Buffer* shadowrays = nullptr;
		RadeonRays::Buffer* shadowhits = nullptr;
		RadeonRays::Buffer* hits = nullptr;
		RadeonRays::Buffer* intersections = nullptr;
		RadeonRays::Buffer* hitcount = nullptr;				// types ?



	};




	PtRenderer::PtRenderer(int devidx, int num_bounces) : renderData(new RenderData), numOfBounces(num_bounces), sceneTracker(devidx) {


		// Allocate space ?


		// init HQ ?


		// scene tracking

		//sceneTracker.compileSceneTest();

	}

	Output * PtRenderer::createOutput(std::uint32_t w, std::uint32_t h) const {
		return new RenderOutput(w, h);
	}

	void PtRenderer::deleteOutput(Output * output) const {
		delete output;
	}

	void PtRenderer::clear(RadeonRays::float3 const & val, Output & output) const {
		throw std::runtime_error("Yet to be done");
	}

	void PtRenderer::preprocess(Scene const & scene) {
	}

	// this is the entry point of the main path tracing algorithm
	void PtRenderer::render(Scene const & scene) {

		auto api = sceneTracker.getIntersectionApi();
		sceneTracker.compileSceneTest(scene);

		// ray gen ?
		generatePrimaryRays(scene);

		std::copy(renderData->host_iota.begin(), renderData->host_iota.end(), renderData->host_pixelIndex[0].begin());
		std::copy(renderData->host_iota.begin(), renderData->host_iota.end(), renderData->host_pixelIndex[1].begin());
	
		// Number of rays 
		int maxrays = renderOutPtr->getWidth() * renderOutPtr->getHeight();
		renderData->host_hitcount = maxrays;

		for (std::uint32_t pass = 0; pass < numOfBounces; ++pass) {

			//... 

			/*// test

			RadeonRays::Event* rayGenEvent = nullptr;
			RadeonRays::ray* rawRayPtr = renderData->host_rays[0].data();

			auto api{ sceneTracker.getIntersectionApi() };

			// memory map
			api->MapBuffer(renderData->rays[0], RadeonRays::kMapRead, 0, maxrays * sizeof(RadeonRays::ray), reinterpret_cast<void**>(&rawRayPtr), &rayGenEvent);
			rayGenEvent->Wait();
			api->DeleteEvent(rayGenEvent);
			rayGenEvent = nullptr;

			std::cerr << "RAY CHECK" << std::endl;
			for (int i = 0; i < 10; ++i) {
				std::cerr << rawRayPtr[i].o.x << '\n';
			}

			api->UnmapBuffer(renderData->rays[0], static_cast<void*>(rawRayPtr), &rayGenEvent);
			rayGenEvent->Wait();
			api->DeleteEvent(rayGenEvent);

			// EOF */

			// T and I here
			//api->QueryIntersection(renderData->rays[pass & 0x1], renderData->hitcount, maxrays, renderData->intersections, nullptr, nullptr);

			api->QueryIntersection(renderData->rays[pass & 0x1], maxrays, renderData->intersections, nullptr, nullptr);

			RadeonRays::Event* mapEvent = nullptr;
			RadeonRays::Intersection* resultPtr = nullptr;

			api->MapBuffer(renderData->intersections, RadeonRays::kMapRead, 0, maxrays * sizeof(RadeonRays::Intersection), reinterpret_cast<void**>(&resultPtr), &mapEvent);
			mapEvent->Wait();
			api->DeleteEvent(mapEvent);

			// copy intersections
			std::copy(resultPtr, resultPtr + maxrays, renderData->host_intersections.begin());


			// tmp
			int count = 0;
			std::cerr << "PRINT\n";
			for (int i = 0; i < maxrays; ++i) {
				if (renderData->host_intersections[i].shapeid != -1) {
					++count;
				}
			}

			std::cerr << "alive: " << count << '\n';

			mapEvent = nullptr;
			api->UnmapBuffer(renderData->intersections, static_cast<void*>(resultPtr), &mapEvent);
			mapEvent->Wait();
			api->DeleteEvent(mapEvent);
			

			// evaluate


			filterPathStream(pass);


			// compact

			restorePixelIndices(pass);

			// rendering here
		}

		
		std::cerr << "Frame: " << frameCount << '\n';

		++frameCount;
	}

	void PtRenderer::setOutput(Output * output) {

		if (!renderOutPtr || renderOutPtr->getWidth() < output->getWidth() || renderOutPtr->getHeight() < output->getHeight()) {
			resizeWorkingSet(*output);
		}

		// KAOCC: check the type !
		renderOutPtr = static_cast<RenderOutput*>(output);
	}





	void PtRenderer::generatePrimaryRays(const Scene& scene) {


		const uint32_t imageWidth = renderOutPtr->getWidth();
		const uint32_t imageHeight = renderOutPtr->getHeight();

		uint32_t rngseed = RadeonRays::rand_uint();

		for (uint32_t y = 0; y < imageHeight; ++y) {
			for (uint32_t x = 0; x < imageWidth; ++x) {			// check this !

				uint32_t seed = x + imageWidth * y * rngseed;
				Sampler randomSampler;
				randomSampler.index = seed;
				randomSampler.scramble = 0;
				randomSampler.dimension = 0;


				RadeonRays::float2 sampleBase;
				sampleBase.x = UniformSampler_Sample1D(&randomSampler);
				sampleBase.y = UniformSampler_Sample1D(&randomSampler);

				RadeonRays::float2 imageSample;
				imageSample.x = (float)x / imageWidth + sampleBase.x / imageWidth;
				imageSample.y = (float)y / imageHeight + sampleBase.y / imageHeight;

				// Transform into [-0.5, 0.5]
				RadeonRays::float2 hSample = imageSample - RadeonRays::float2(0.5f, 0.5f);

				// Transform into [-dim/2, dim/2]		
				const PerspectiveCamera* cameraPtr = static_cast<const PerspectiveCamera*>(scene.getCamera());  // check this 
				RadeonRays::float2 cSample = hSample * cameraPtr->getSensorSize();


				// set ray
				RadeonRays::ray& currentRay = renderData->host_rays[0][y * imageWidth + x];

				currentRay.d = RadeonRays::normalize(cameraPtr->getFocalLength() * cameraPtr->getForwardVector() + cSample.x * cameraPtr->getRightVector() + cSample.y * cameraPtr->getUpVector());
				currentRay.o = cameraPtr->getPosition() + cameraPtr->getDepthRange().x * currentRay.d;

				currentRay.o.w = cameraPtr->getDepthRange().y - cameraPtr->getDepthRange().x;
				currentRay.d.w = sampleBase.x;		// check

				currentRay.extra.x = 0xFFFFFFFF;
				currentRay.extra.y = 0xFFFFFFFF;
				currentRay.padding = currentRay.extra;


				//std::cerr << "Ray d x: " << currentRay.d.x << '\n';
				//std::cerr << "Ray o y: " << currentRay.o.x << '\n';

				// ...

				// path ?
				Path& path = renderData->host_path[y * imageWidth + x];
				path.initGen();
			}
		}

		RadeonRays::Event* rayGenEvent = nullptr;
		RadeonRays::ray* rawRayPtr = renderData->host_rays[0].data();

		auto api{ sceneTracker.getIntersectionApi() };

		// memory map
		api->MapBuffer(renderData->rays[0], RadeonRays::kMapWrite, 0, imageWidth * imageHeight * sizeof(RadeonRays::ray), reinterpret_cast<void**>(&rawRayPtr), &rayGenEvent);
		rayGenEvent->Wait();
		api->DeleteEvent(rayGenEvent);
		rayGenEvent = nullptr;

		// to RR memory
		std::copy(renderData->host_rays[0].begin(), renderData->host_rays[0].end(), rawRayPtr);

		api->UnmapBuffer(renderData->rays[0], static_cast<void*>(rawRayPtr), &rayGenEvent);
		rayGenEvent->Wait();
		api->DeleteEvent(rayGenEvent);
		

		//throw std::runtime_error("Gen: Yet to be done");
	}

	void PtRenderer::resizeWorkingSet(const Output& out) {

		// clear and resize
		renderData->host_rays[0].clear();
		renderData->host_rays[0].resize(out.getWidth() * out.getHeight());

		renderData->host_rays[1].clear();
		renderData->host_rays[1].resize(out.getWidth() * out.getHeight());

		renderData->host_shadowrays.clear();
		renderData->host_shadowrays.resize(out.getWidth() * out.getHeight());

		renderData->host_shadowhits.clear();
		renderData->host_shadowhits.resize(out.getWidth() * out.getHeight());

		renderData->host_intersections.clear();
		renderData->host_intersections.resize(out.getWidth() * out.getHeight());

		renderData->host_hits.clear();
		renderData->host_hits.resize(out.getWidth() * out.getHeight());

		renderData->host_iota.clear();
		renderData->host_iota.resize(out.getWidth() * out.getHeight());
		std::iota(renderData->host_iota.begin(), renderData->host_iota.end(), 0);


		renderData->host_pixelIndex[0].clear();
		renderData->host_pixelIndex[0].resize(out.getWidth() * out.getHeight());

		renderData->host_pixelIndex[1].clear();
		renderData->host_pixelIndex[1].resize(out.getWidth() * out.getHeight());

		renderData->host_compactedIndex.clear();
		renderData->host_compactedIndex.resize(out.getWidth() * out.getHeight());


		//renderData->host_hitcount.clear();
		//renderData->host_hitcount.resize(out.getWidth() * out.getHeight());

		renderData->host_hitcount = 0;

		renderData->host_path.clear();
		renderData->host_path.resize(out.getWidth() * out.getHeight());

		auto api{ sceneTracker.getIntersectionApi() };

		// Delete buffer connections
		api->DeleteBuffer(renderData->rays[0]);
		api->DeleteBuffer(renderData->rays[1]);
		api->DeleteBuffer(renderData->shadowrays);
		api->DeleteBuffer(renderData->shadowhits);
		api->DeleteBuffer(renderData->hits);
		api->DeleteBuffer(renderData->intersections);
		api->DeleteBuffer(renderData->hitcount);

		// Create new buffers
		// KaoCC: be careful of the size
		renderData->rays[0] = api->CreateBuffer(out.getWidth() * out.getHeight()  * sizeof(RadeonRays::ray), renderData->host_rays[0].data());
		renderData->rays[1] = api->CreateBuffer(out.getWidth() * out.getHeight() * sizeof(RadeonRays::ray), renderData->host_rays[1].data());
		renderData->shadowrays = api->CreateBuffer(out.getWidth() * out.getHeight() * sizeof(RadeonRays::ray), renderData->host_shadowrays.data());
		renderData->shadowhits = api->CreateBuffer(out.getWidth() * out.getHeight() * sizeof(int), renderData->host_shadowhits.data());
		renderData->intersections = api->CreateBuffer(out.getWidth() * out.getHeight() * sizeof(RadeonRays::Intersection), renderData->host_intersections.data());
		renderData->hits = api->CreateBuffer(out.getWidth() * out.getHeight() * sizeof(int), renderData->host_hits.data());
		renderData->hitcount = api->CreateBuffer( sizeof(int), &renderData->host_hitcount);

		// ......

		//throw std::runtime_error("Yet to be done !");

	}

	void PtRenderer::evaluateVolume(int pass) {

		std::vector<RadeonRays::ray>& rayArrayRef = renderData->host_rays[pass & 0x1];
		std::vector<int>& pixelIndexArrayRef = renderData->host_pixelIndex[(pass + 1) & 0x1];

		for (size_t i = 0; i < renderData->host_hitcount; ++i) {  // Need to check the size

			int pixelIndex = pixelIndexArrayRef[i];

			// Path ?
			Path& path = renderData->host_path[pixelIndex];

			if (!path.isAlive()) {
				continue;
			}

			int volumIdx = path.getVolumeIdx();
			if (volumIdx != -1) {

			}


		}

		throw std::runtime_error("Yet to be done !");

	}



	void PtRenderer::restorePixelIndices(int pass) {

		std::vector<int>& previousPixelIndexArrayRef = renderData->host_pixelIndex[(pass + 1) & 0x1];
		std::vector<int>& newPixelIndexArrayRef = renderData->host_pixelIndex[(pass) & 0x1];

		for (size_t i = 0; i < renderData->host_hitcount; ++i) {		// check upper bound ?
			newPixelIndexArrayRef[i] = previousPixelIndexArrayRef[renderData->host_compactedIndex[i]];
		}

	}

	void PtRenderer::filterPathStream(int pass) {

		std::vector<int>& pixelIndexArrayRef = renderData->host_pixelIndex[(pass + 1) & 0x1];

		for (size_t i = 0; i < renderData->host_hitcount; ++i) {		// check the upper limit

			int pixelIndex = pixelIndexArrayRef[i];
			Path& path = renderData->host_path[pixelIndex];

			if (path.isAlive()) {

				// TMP !!!! 
				// We should check the contribution
				bool kill = false;

				if (!kill) {

					renderData->host_hits[i] = renderData->host_intersections[i].shapeid >= 0 ? 1 : 0;

				} else {
					path.kill();
					renderData->host_hits[i] = 0;
				}

			} else {
				renderData->host_hits[i] = 0;
			}


		}


	}

	void PtRenderer::compactIndex() {

		const size_t  maxSize = renderData->host_hits.size();

		for (size_t i = 0; i < maxSize; ++i) {  // KAOCC: check the upper limit

			//renderData->host_hitcount[] = renderData->host_iota[i];



		}


	}

}