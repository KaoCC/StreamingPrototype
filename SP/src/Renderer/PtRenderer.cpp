#include "PtRenderer.hpp"
#include "Sampler.hpp"

#include "Scene/Camera.hpp"


#include "DifferentialGeometry.hpp"

#include "BxDFHelper.hpp"

#include "Scene/Light.hpp"

#include <iostream>
#include <numeric>

#include "MathUtility.hpp"

#include "RandomSampler.hpp"
#include "CorrelatedMultiJitterSampler.hpp"

namespace SP {


	//struct PtRenderer::PathState {
	//	RadeonRays::float4 throughput;
	//	int volume;
	//	int flags;
	//	int extra_0;
	//	int extra_1;
	//};


	// helper function
	static constexpr float transformColorToLuminance(float x, float y, float z) {
		return x * 0.2126f + y * 0.7152f + z * 0.0722f;
	}

	// FIXME: move this part to proper class 
	// helper function
	static void selectMaterial(const RadeonRays::float3& wi, const Sampler& sampler, DifferentialGeometry& diffGeo) {

		auto* mat = dynamic_cast<const MultiBxDF*>(diffGeo.getOriginalMaterial());

		// handle MultiBxDF only
		if (mat != nullptr) {


			// TMP !!!
			// FIXME: select the proper material

			if (BxDFHelper::isEmissive(mat)) {

				auto diffusePart = mat->getInputValue("base_material");
				diffGeo.setCurrentMaterial(diffusePart.matValue);
			} else {

				// testing only
				if (std::rand() % 10 == 0) {
					auto specularPart = mat->getInputValue("top_material");
					diffGeo.setCurrentMaterial(specularPart.matValue);
				} else {
					auto diffusePart = mat->getInputValue("base_material");
					diffGeo.setCurrentMaterial(diffusePart.matValue);
				}
			}

			// setting fresnel ?

		}

		// TODO: setup and calculate fresnel 

	}


	PtRenderer::~PtRenderer() = default;


	PtRenderer::PtRenderer(unsigned num_bounces, ApiEngine& engine) :
			numOfBounces { num_bounces }, mEngineRef { engine } {

	}

	//std::shared_ptr<Output> PtRenderer::createOutput(std::uint32_t w, std::uint32_t h) const {
	//	return std::make_shared<RenderOutput>(w, h);
	//}

	//void PtRenderer::deleteOutput(Output * output) const {
	//	delete output;
	//}



	void PtRenderer::preprocess(Scene const& scene) {
	}

	// this is the entry point of the main path tracing algorithm
	void PtRenderer::render(Scene const& scene, size_t configIdx) {

		//auto api = sceneTracker.getIntersectionApi();
		//sceneTracker.compileSceneTest(scene);

		//mEngineRef->compileScene(scene);


		// ray gen ?
		generatePrimaryRays(scene, configIdx);

		std::copy(renderData.host_iota.begin(), renderData.host_iota.end(), renderData.host_pixelIndex[0].begin());
		std::copy(renderData.host_iota.begin(), renderData.host_iota.end(), renderData.host_pixelIndex[1].begin());

		// Number of rays 
		int maxrays = renderOutPtr->getWidth() * renderOutPtr->getHeight();
		renderData.host_hitcount = maxrays;


		for (unsigned pass = 0; pass < numOfBounces; ++pass) {

			// clear hit buffer
			std::fill(renderData.host_hits.begin(), renderData.host_hits.end(), 0);

			//api->QueryIntersection(renderData.rays[pass & 0x1], renderData.host_hitcount, renderData.intersections, nullptr, nullptr);		

			mEngineRef.queryIntersection(renderData.host_rays[pass & 0x1], renderData.host_hitcount, renderData.host_intersections).wait();


			// evaluate V		(X)

			filterPathStream(pass);

			compactIndex();
			//std::cerr << "NEW hit count:" << renderData.host_hitcount << '\n';

			restorePixelIndices(pass);


			// --- RENDERING ---

			// Shade V		(X)

			// Shade Surface			// the key of rendering !
			shadeSurface(scene, pass);

			if (pass == 0) {
				shadeMiss(pass);
			}


			// tmp
			if (renderData.host_hitcount == 0) {
				std::cerr << ">>> BREAK AT PASS: " << pass << "\n";
				break;
			}

			// QueryOcclusion
			// KAOCC: move shadow rays to the GPU ?
			// Read the shadow hit to host ?
			// hit count ? max ray ?

			//api->QueryOcclusion(renderData.shadowrays, renderData.host_hitcount, renderData.shadowhits, nullptr, nullptr);
			mEngineRef.queryOcclusion(renderData.host_shadowrays, renderData.host_hitcount, renderData.host_shadowhits).wait();


			//// test
			//int shadowCount = 0;
			//for (int i = 0; i < renderData.host_hitcount; ++i) {
			//	if (renderData.host_shadowhits[i] != -1) {
			//		++shadowCount;

			//		//std::cerr << ">>>>>>>>>>>>>>>>>>> shape: " << renderData.host_intersections[i].shapeid <<" Pri: " << renderData.host_intersections[i].primid << '\n';
			//	}
			//}

			//std::cerr << "Shadow: " << shadowCount << '\n';

			gatherLightSamples(pass);

		}

		//std::cerr << "Frame: " << frameCount << '\n';

		++mFrameCount;
	}

	void PtRenderer::setOutput(std::shared_ptr<RenderOutput> output) {

		if (!renderOutPtr || renderOutPtr->getWidth() < output->getWidth() || renderOutPtr->getHeight() < output->getHeight()) {
			resizeWorkingSet(*output);
		}

		renderOutPtr = output;
	}


	void PtRenderer::generatePrimaryRays(const Scene& scene, size_t camIdx, bool useSampler) {


		const auto imageWidth = renderOutPtr->getWidth();
		const auto imageHeight = renderOutPtr->getHeight();

		auto rngseed = RadeonRays::rand_uint();

		for (std::size_t y = 0; y < imageHeight; ++y) {
			for (std::size_t x = 0; x < imageWidth; ++x) {            // check this !



				//Sampler randomSampler;
				//randomSampler.index = seed;
				//randomSampler.scramble = 0;
				//randomSampler.dimension = 0;


				const std::uint32_t seed = x + imageWidth * y * rngseed;
				std::unique_ptr<Sampler> sampler = RandomSampler::create(seed);

				//unsigned rnd = RadeonRays::rand_uint();		// test !
				//unsigned scramble = rnd * 0x1fe3434f * ((frameCount + 133 * rnd) / (CorrelatedMultiJitterSampler::kDim * CorrelatedMultiJitterSampler::kDim));

				//std::unique_ptr<Sampler> sampler = CorrelatedMultiJitterSampler::create(frameCount % (CorrelatedMultiJitterSampler::kDim  * CorrelatedMultiJitterSampler::kDim), 0, scramble);


				RadeonRays::float2 sampleBase = sampler->sample2D();


				RadeonRays::float2 imageSample;
				imageSample.x = (float) x / imageWidth;
				imageSample.y = (float) y / imageHeight;

				if (useSampler) {
					imageSample.x += sampleBase.x / imageWidth;
					imageSample.y += sampleBase.y / imageHeight;
				}

				// Transform into [-0.5, 0.5]
				RadeonRays::float2 hSample = imageSample - RadeonRays::float2(0.5f, 0.5f);

				// Transform into [-dim/2, dim/2]		
				const PerspectiveCamera& cameraRef { static_cast<const PerspectiveCamera&>(scene.getCamera(camIdx)) };  // check this
				RadeonRays::float2 cSample = hSample * cameraRef.getSensorSize();


				// set ray
				RadeonRays::ray& currentRay = renderData.host_rays[0][y * imageWidth + x];

				currentRay.d = RadeonRays::normalize(cameraRef.getFocalLength() * cameraRef.getForwardVector() + cSample.x * cameraRef.getRightVector() +
													 cSample.y * cameraRef.getUpVector());
				currentRay.o = cameraRef.getPosition() + cameraRef.getDepthRange().x * currentRay.d;

				currentRay.o.w = cameraRef.getDepthRange().y - cameraRef.getDepthRange().x;

				if (useSampler) {
					currentRay.d.w = sampleBase.x;        // check
				}
				else {
					// TODO: what to do when no use sampler?
					currentRay.d.w = 1;
				}
				
				

				currentRay.SetMaxT(std::numeric_limits<float>::max());
				currentRay.SetTime(0.f);
				currentRay.SetMask(0xFFFFFFFF);
				currentRay.SetActive(true);


				//currentRay.SetMask(0xFFFFFFFFF);
				//currentRay.extra.x = 0xFFFFFFFF;
				//currentRay.extra.y = 0xFFFFFFFF;
				//currentRay.padding = currentRay.extra;


				//std::cerr << "Ray d x: " << currentRay.d.x << '\n';
				//std::cerr << "Ray o y: " << currentRay.o.x << '\n';

				// ...

				// path ?
				Path& path { renderData.host_path[y * imageWidth + x] };
				path.initGen();
			}
		}

	}

	void PtRenderer::resizeWorkingSet(const Output& out) {

		// clear and resize
		renderData.host_rays[0].clear();
		renderData.host_rays[0].resize(out.getWidth() * out.getHeight());

		renderData.host_rays[1].clear();
		renderData.host_rays[1].resize(out.getWidth() * out.getHeight());

		renderData.host_shadowrays.clear();
		renderData.host_shadowrays.resize(out.getWidth() * out.getHeight());

		renderData.host_shadowhits.clear();
		renderData.host_shadowhits.resize(out.getWidth() * out.getHeight());

		renderData.host_intersections.clear();
		renderData.host_intersections.resize(out.getWidth() * out.getHeight());

		renderData.host_hits.clear();
		renderData.host_hits.resize(out.getWidth() * out.getHeight());

		renderData.host_iota.clear();
		renderData.host_iota.resize(out.getWidth() * out.getHeight());
		std::iota(renderData.host_iota.begin(), renderData.host_iota.end(), 0);


		renderData.host_pixelIndex[0].clear();
		renderData.host_pixelIndex[0].resize(out.getWidth() * out.getHeight());

		renderData.host_pixelIndex[1].clear();
		renderData.host_pixelIndex[1].resize(out.getWidth() * out.getHeight());

		renderData.host_compactedIndex.clear();
		renderData.host_compactedIndex.resize(out.getWidth() * out.getHeight());

		renderData.host_hitcount = 0;

		renderData.host_path.clear();
		renderData.host_path.resize(out.getWidth() * out.getHeight());

		renderData.host_lightSamples.clear();
		renderData.host_lightSamples.resize(out.getWidth() * out.getHeight());

	}

	void PtRenderer::renderDepthMap(Scene const & scene, size_t configIdx)
	{
		//resizeWorkingSet(*renderOutPtr);
		// ray gen 
		generatePrimaryRays(scene, configIdx, false); // do not use sampler for depth map generation

		std::copy(renderData.host_iota.begin(), renderData.host_iota.end(), renderData.host_pixelIndex[0].begin());
		std::copy(renderData.host_iota.begin(), renderData.host_iota.end(), renderData.host_pixelIndex[1].begin());

		// Number of rays 
		int maxrays = renderOutPtr->getWidth() * renderOutPtr->getHeight();
		renderData.host_hitcount = maxrays;

		// clear hit buffer
		std::fill(renderData.host_hits.begin(), renderData.host_hits.end(), 0);

		// query nearest intersection
		mEngineRef.queryIntersection(renderData.host_rays[0], renderData.host_hitcount, renderData.host_intersections).wait();

		filterPathStream(0);

		compactIndex();

		restorePixelIndices(0);

		// compute the hit into depth
		computeDepthMap(scene);
	}
	
	void PtRenderer::computeDepthMap(const Scene& scene) {
		
		auto& outRef = *renderOutPtr;
		const std::vector<RadeonRays::ray>& rayArrayRef = renderData.host_rays[0];
		const std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[0];
		auto& depthDataRef = outRef.getDepthData();
		// default to nearest value: 0
		std::fill(depthDataRef.begin(), depthDataRef.end(), 0);
		for (size_t i = 0; i < renderData.host_hitcount; ++i) {

			size_t hitIndex = renderData.host_compactedIndex[i];
			size_t pixelIndex = pixelIndexArrayRef[i];
			const RadeonRays::Intersection& currentIntersect = renderData.host_intersections[hitIndex];
			DifferentialGeometry diffGeo{ currentIntersect, scene };
			auto& ray = rayArrayRef[hitIndex];

			auto& hitPosition = diffGeo.getPosition();
			float distance = std::sqrt((hitPosition - ray.o).sqnorm());

			depthDataRef[pixelIndex] = distance;
		}

	}


	void PtRenderer::shadeSurface(const Scene& scene, unsigned pass) {


		const std::vector<RadeonRays::ray>& rayArrayRef = renderData.host_rays[pass & 0x1];
		auto& indirectRayArrayRef = renderData.host_rays[(pass + 1) & 0x1];

		auto& shadowRayArrayRef = renderData.host_shadowrays;
		auto& lightSamplesArrayRef = renderData.host_lightSamples;
		auto& outRef = *renderOutPtr;

		const std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[(pass) & 0x1];

		std::uint32_t rngseed = RadeonRays::rand_uint();

		//DifferentialGeometry diffGeo;

		for (size_t i = 0; i < renderData.host_hitcount; ++i) {

			size_t hitIndex = renderData.host_compactedIndex[i];
			size_t pixelIndex = pixelIndexArrayRef[i];
			const RadeonRays::Intersection& currentIntersect = renderData.host_intersections[hitIndex];

			Path& currentPath = renderData.host_path[pixelIndex];

			if (currentPath.isScattered()) {
				continue;
			}

			// Incoming direction
			const RadeonRays::float3& wi = -RadeonRays::normalize(rayArrayRef[hitIndex].d);

			// init sampler (random)
			// TODO: change this to a function
			std::uint32_t seed = pixelIndex * rngseed;
			//Sampler randomSampler;
			//randomSampler.index = seed;
			//randomSampler.scramble = 0;
			//randomSampler.dimension = 0;

			//unsigned rnd = RadeonRays::rand_uint();
			//unsigned scramble = rnd * 0x1fe3434f * ((frameCount + 331 * rnd) / (CorrelatedMultiJitterSampler::kDim * CorrelatedMultiJitterSampler::kDim));
			//std::unique_ptr<Sampler> sampler = CorrelatedMultiJitterSampler::create(frameCount % (CorrelatedMultiJitterSampler::kDim  * CorrelatedMultiJitterSampler::kDim), 4 + numOfBounces * 300, scramble);

			std::unique_ptr<Sampler> sampler = RandomSampler::create(seed);


			// KAOCC: how to change this ?
			//DifferentialGeometry diffGeo;
			DifferentialGeometry diffGeo { currentIntersect, scene };




			bool backfaced = (RadeonRays::dot(diffGeo.getNormal(), wi) < 0);
			bool twosided = diffGeo.getCurrentMaterial()->isTwoSided();

			if (backfaced && twosided) {
				// invert normal
				diffGeo.setNormal(-diffGeo.getNormal());

				// dpdu ???
				diffGeo.setDpDu(-diffGeo.getDpDu());

				// dpdv ???
				diffGeo.setDpDu(-diffGeo.getDpDv());
			}


			// Select BxDF material ?
			// Note: Mat select is broken ...

			// TODO: select Material here ... !
			selectMaterial(wi, *sampler, diffGeo);



			if (BxDFHelper::isEmissive(diffGeo.getCurrentMaterial())) {

				if (!backfaced) {

					float weight = 1.0f;

					if (pass > 0 && !currentPath.isSpecular()) {

						// test code
						//RadeonRays::float2 extra = (rayArrayRef[hitIndex].extra);
						//float ld = currentIntersect.uvwt.w;
						//float denom = extra.y * diffGeo.getArea();
						//float bxdflightpdf = denom > 0.f ? (ld * ld / denom / 1) : 0.f;
						//weight = balanceHeuristic(1, extra.x, 1, bxdflightpdf);

						//std::cerr << "WWWWWW: " << weight <<'\n';
					}


					outRef[pixelIndex] += currentPath.getThroughput() * BxDFHelper::getEmissiveLe(diffGeo) * weight;
				}



				// kill the path and disable rays
				currentPath.kill();
				shadowRayArrayRef[i].SetActive(false);
				indirectRayArrayRef[i].SetActive(false);


				// light sample ?
				lightSamplesArrayRef[i] = 0.f;

				// CHECK !
				continue;
			}


			//float n_dot_wi = RadeonRays::dot(diffGeo.getNormal(), wi);


			if (!twosided && backfaced && !BxDFHelper::isBTDF(diffGeo.getCurrentMaterial())) {
				// invert normal
				diffGeo.setNormal(-diffGeo.getNormal());

				// dpdu ???
				diffGeo.setDpDu(-diffGeo.getDpDu());

				// dpdv ???
				diffGeo.setDpDu(-diffGeo.getDpDv());
			}


			// >>>>>>>>>>>>> DiffGeo Tangert Trans?
			diffGeo.calculateTangentTransform();

			// PDFs 
			float lightPDF = 0.f;
			float bxdflightPDF = 0.f;
			float bxdfPDF = 0.f;
			float lightbxdfPDF = 0.f;
			float selectionPDF = 0.f;

			// Outs
			RadeonRays::float3 lightwo;
			RadeonRays::float3 bxdfwo;
			RadeonRays::float3 wo;

			// Weight 
			float bxdfWeight = 1.f;
			float lightWeight = 1.f;

			// sample BxDf
			const RadeonRays::float3& bxdf = BxDFHelper::sample(diffGeo, wi, sampler->sample2D(), bxdfwo, bxdfPDF);        // retrun value not used ?

			const auto currentScenePtr = &scene;

			// Radiance
			RadeonRays::float3 radiance = 0.f;

			// light ?
			const Light* lightInst = currentScenePtr->getSampleLightPtr(sampler->sample1D(), selectionPDF);        // check the light index !
			if (lightInst != nullptr) {

				RadeonRays::float3 currentLe = lightInst->sample(diffGeo, sampler->sample2D(), lightwo, lightPDF);

				// getPDF is missing here

				lightWeight = lightInst->isSingular() ? 1.f : 0.f;            // CHECK !

				if (currentLe.sqnorm() > 0 && lightPDF > 0.0f && !BxDFHelper::isSingular(diffGeo.getCurrentMaterial())) {
					wo = lightwo;
					float n_dot_wo = std::abs(dot(diffGeo.getNormal(), normalize(wo)));


					// times light weight ?
					radiance = currentLe * BxDFHelper::evaluate(diffGeo, wi, RadeonRays::normalize(wo)) * currentPath.getThroughput() * n_dot_wo * (1 / lightPDF) * (1 / selectionPDF);

					//std::cerr << "Radiance : " << radiance.x << " " << radiance.y << " " << radiance.z << "\n";
				}

			}


			// radiance ?
			if (radiance.sqnorm() > 0.f) {
				// Generate shadow rays 

				const RadeonRays::float3& shadowRay_orig = diffGeo.getPosition() + 0.001f * diffGeo.getNormal(); // +LOW_DISTANCE * s * diffGeo.getNormal();
				const RadeonRays::float3& shadowRay_dir = RadeonRays::normalize(wo);

				shadowRayArrayRef[i] = RadeonRays::ray(shadowRay_orig, shadowRay_dir);

				lightSamplesArrayRef[i] = radiance;

			} else {
				shadowRayArrayRef[i].SetActive(false);
				lightSamplesArrayRef[i] = 0.f;

			}


			// Apply Russian roulette
			// Luminance
			float qq = std::max(std::min(0.5f, transformColorToLuminance(currentPath.getThroughput().x, currentPath.getThroughput().y, currentPath.getThroughput().z)), 0.01f);
			bool rrApplyFlag = pass > 3;
			bool rrStopFlag = (sampler->sample1D() > qq) && rrApplyFlag;        // value ?		// wrong !

			//Sampler_Sample1D(&randomSampler) > qq
			// ...

			if (rrApplyFlag) {
				currentPath.multiplyThroughput(1 / qq);
			}


			if (BxDFHelper::isSingular(diffGeo.getCurrentMaterial())) {
				currentPath.setSpecularFlag();
			}

			// handle indirectrays
			bxdfwo = normalize(bxdfwo);
			const RadeonRays::float3& t = bxdf * std::abs(RadeonRays::dot(diffGeo.getNormal(), bxdfwo));        // value ?

			//std::cerr << "T: " << t.sqnorm()  << " bxdfPDF: " << bxdfPDF << std::endl;

			if (t.sqnorm() > 0 && bxdfPDF > 0.f && !rrStopFlag) {

				//std::cerr << " >>>>>>>>>>>>>>>>>> NEW VALUE ! " << i <<"\n";

				currentPath.multiplyThroughput(t * (1 / bxdfPDF));

				// Generate indirect ray
				const RadeonRays::float3& indirectRay_orig = diffGeo.getPosition() + 0.001f * diffGeo.getNormal(); // + LOW_DISTANCE * s * diffgeo.n;
				const RadeonRays::float3& indirectRay_dir = bxdfwo;

				indirectRayArrayRef[i] = RadeonRays::ray(indirectRay_orig, indirectRay_dir);

			} else {
				currentPath.kill();
				indirectRayArrayRef[i].SetActive(false);
			}


		}

	}

	// not used ...
	void PtRenderer::evaluateVolume(unsigned pass) {

		std::vector<RadeonRays::ray>& rayArrayRef = renderData.host_rays[pass & 0x1];
		std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[(pass + 1) & 0x1];

		for (size_t i = 0; i < renderData.host_hitcount; ++i) {  // Need to check the size

			int pixelIndex = pixelIndexArrayRef[i];

			// Path ?
			Path& path = renderData.host_path[pixelIndex];

			if (!path.isAlive()) {
				continue;
			}

			int volumIdx = path.getVolumeIdx();
			if (volumIdx != -1) {


				//Sampler rendomSampler;
				uint32_t rngseed = RadeonRays::rand_uint();            // check this !
				uint32_t seed = pixelIndex * rngseed;
				//Sampler randomSampler;
				//randomSampler.index = seed;
				//randomSampler.scramble = 0;
				//randomSampler.dimension = 0;



				// Try sampling volume for a next scattering event
				float pdf = 0.f;
				//float maxdist = Intersection_GetDistance(isects + globalid);
				//float d = Volume_SampleDistance(&volumes[volidx], &rays[globalid], maxdist, Sampler_Sample1D(&sampler, SAMPLER_ARGS), &pdf);

				// Check if we shall skip the event (it is either outside of a volume or not happened at all)
				//bool skip = d < 0 || d > maxdist || pdf <= 0.f;

				//if (skip) {

				//	path.clearScatterFlag();

				//} else {

				//	path.setScatterFlag();

				//}




			}


		}

		throw std::runtime_error("Yet to be done !");

	}


	void PtRenderer::shadeMiss(unsigned pass) {

		const std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[(pass + 1) & 0x1];

		const size_t maxSize = renderOutPtr->getWidth() * renderOutPtr->getHeight();        // check the size !!
		for (size_t i = 0; i < maxSize; ++i) {

			int pixelIndex = pixelIndexArrayRef[i];


			// this part is not complete yet
			// texture is not support yet
			/*  Test Area */
			if (renderData.host_intersections[i].shapeid < 0) {

				int volumeIndex = renderData.host_path[pixelIndex].getVolumeIdx();

				auto& outRef = *renderOutPtr;

				if (volumeIndex == -1) {

					//outRef[pixelIndex] = ;

				} else {
					// not support currently
					throw std::runtime_error("Vol not support");
				}

				// Yet to be done
			}

			/* EOF */



			auto& outRef = *renderOutPtr;
			outRef[pixelIndex].w += 1.f;
		}


	}

	void PtRenderer::gatherLightSamples(unsigned pass) {


		const std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[(pass) & 0x1];
		const auto& lightSamplesArrayRef = renderData.host_lightSamples;
		auto& outRef = *renderOutPtr;

		for (size_t i = 0; i < renderData.host_hitcount; ++i) {        // check upper bound !
			int pixelIndex = pixelIndexArrayRef[i];

			RadeonRays::float3 radiance = RadeonRays::float3(0.f, 0.f, 0.f);

			if (renderData.host_shadowhits[i] == -1) {
				// test
				//std::cerr << "shadow: " << i << '\n';

				radiance += lightSamplesArrayRef[i];// TEST !!!!!!!!
			}


			outRef[pixelIndex] += radiance;
		}

		// yet to be done !


	}


	void PtRenderer::restorePixelIndices(unsigned pass) {

		const std::vector<int>& previousPixelIndexArrayRef = renderData.host_pixelIndex[(pass + 1) & 0x1];
		std::vector<int>& newPixelIndexArrayRef = renderData.host_pixelIndex[(pass) & 0x1];

		for (size_t i = 0; i < renderData.host_hitcount; ++i) {        // check upper bound ?
			newPixelIndexArrayRef[i] = previousPixelIndexArrayRef[renderData.host_compactedIndex[i]];
		}

	}


	void PtRenderer::filterPathStream(unsigned pass) {

		const std::vector<int>& pixelIndexArrayRef = renderData.host_pixelIndex[(pass + 1) & 0x1];

		for (size_t i = 0; i < renderData.host_hitcount; ++i) {        // check the upper limit

			int pixelIndex = pixelIndexArrayRef[i];
			Path& path = renderData.host_path[pixelIndex];

			if (path.isAlive()) {

				// TMP !!!! 
				// We should check the contribution
				bool kill = false;

				if (!kill) {

					renderData.host_hits[i] = (renderData.host_intersections[i].shapeid >= 0) ? 1 : 0;

				} else {
					path.kill();
					renderData.host_hits[i] = 0;
				}

			} else {
				renderData.host_hits[i] = 0;
			}

		}

	}

	void PtRenderer::compactIndex() {

		const size_t maxSize = renderData.host_hits.size();

		//Exclusive scan add 
		// ...

		std::vector<int> address(maxSize);
		scanExclusiveAdd(address);

		for (size_t i = 0; i < maxSize; ++i) {  // KAOCC: check the upper limit

			if (renderData.host_hits[i]) {

				renderData.host_compactedIndex[address[i]] = renderData.host_iota[i];
			}

		}


		renderData.host_hitcount = address[maxSize - 1] + renderData.host_hits[maxSize - 1];
	}

	// simple sequential version
	void PtRenderer::scanExclusiveAdd(std::vector<int>& addr) {

		// KAOCC:
		// TODO: change to partial_sum ?

		const size_t maxSize = addr.size();

		addr[0] = 0;
		for (size_t i = 1; i < maxSize; ++i) {
			addr[i] = renderData.host_hits[i - 1] + addr[i - 1];
		}

	}

}

