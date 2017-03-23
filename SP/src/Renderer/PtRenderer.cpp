#include "PtRenderer.hpp"
#include "Sampler.hpp"

#include "Scene/Camera.hpp"

namespace SP {


	struct PtRenderer::RenderData {


		// host buffers

		std::vector<RadeonRays::ray> host_rays[2];
		std::vector<int> host_hits;

		std::vector<RadeonRays::ray> host_shadowrays;
		std::vector<int> host_shadowhits;

		std::vector<RadeonRays::Intersection> host_intersections;
		std::vector<int> host_hitcount;


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

		for (std::uint32_t pass = 0; pass < numOfBounces; ++pass) {

			//... 

			// T and I here

			// rendering here
		}


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


		const size_t imageWidth = renderOutPtr->getWidth();
		const size_t imageHeight = renderOutPtr->getHeight();

		uint32_t rngseed = RadeonRays::rand_uint();


		for (size_t y = 0; y < imageHeight; ++y) {
			for (size_t x = 0; x < imageWidth; ++x) {			// check this !

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

				// ...

				// path ?
			}
		}



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

		renderData->host_hitcount.clear();
		renderData->host_hitcount.resize(out.getWidth() * out.getHeight());

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
		renderData->rays[0] = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_rays[0].data());
		renderData->rays[1] = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_rays[1].data());
		renderData->shadowrays = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_shadowrays.data());
		renderData->shadowhits = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_shadowhits.data());
		renderData->intersections = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_intersections.data());
		renderData->hits = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_hits.data());
		renderData->hitcount = api->CreateBuffer(out.getWidth() * out.getHeight(), renderData->host_hitcount.data());

		// ......


		//throw std::runtime_error("Yet to be done !");

	}



}