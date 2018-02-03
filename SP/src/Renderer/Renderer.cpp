#include "Renderer.hpp"

#include "Scene/Camera.hpp"
#include "RandomSampler.hpp"

#include <memory>


namespace SP {

	void generateRandomRay(uint32_t rngseed, uint32_t x, uint32_t y, uint32_t imageWidth, uint32_t imageHeight, RadeonRays::ray & currentRay, const PerspectiveCamera& cameraRef) {


		uint32_t seed = x + imageWidth * y * rngseed;
		std::unique_ptr<Sampler> sampler = RandomSampler::create(seed);

		//unsigned rnd = RadeonRays::rand_uint();		// test !
		//unsigned scramble = rnd * 0x1fe3434f * ((frameCount + 133 * rnd) / (CorrelatedMultiJitterSampler::kDim * CorrelatedMultiJitterSampler::kDim));

		//std::unique_ptr<Sampler> sampler = CorrelatedMultiJitterSampler::create(frameCount % (CorrelatedMultiJitterSampler::kDim  * CorrelatedMultiJitterSampler::kDim), 0, scramble);

		RadeonRays::float2 sampleBase = sampler->sample2D();

		RadeonRays::float2 imageSample;
		imageSample.x = (float)x / imageWidth + sampleBase.x / imageWidth;
		imageSample.y = (float)y / imageHeight + sampleBase.y / imageHeight;

		// Transform into [-0.5, 0.5]
		RadeonRays::float2 hSample = imageSample - RadeonRays::float2(0.5f, 0.5f);

		// Transform into [-dim/2, dim/2]		
		//const PerspectiveCamera* cameraPtr = static_cast<const PerspectiveCamera*>(scene.getCamera(camIdx));  // check this 
		RadeonRays::float2 cSample = hSample * cameraRef.getSensorSize();


		// set ray
		//RadeonRays::ray& currentRay = renderData->host_rays[0][y * imageWidth + x];

		currentRay.d = RadeonRays::normalize(cameraRef.getFocalLength() * cameraRef.getForwardVector() + cSample.x * cameraRef.getRightVector() + cSample.y * cameraRef.getUpVector());
		currentRay.o = cameraRef.getPosition() + cameraRef.getDepthRange().x * currentRay.d;

		currentRay.o.w = cameraRef.getDepthRange().y - cameraRef.getDepthRange().x;
		currentRay.d.w = sampleBase.x;		// check

		currentRay.SetMaxT(std::numeric_limits<float>::max());
		currentRay.SetTime(0.f);
		currentRay.SetMask(0xFFFFFFFF);
		currentRay.SetActive(true);
	}




}