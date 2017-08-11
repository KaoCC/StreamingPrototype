#include "Renderer.hpp"

#include "Scene/Camera.hpp"
#include "RandomSampler.hpp"

#include <memory>


namespace SP {

	void generateRandomRay(uint32_t rngseed, uint32_t x, uint32_t y, uint32_t imageWidth, uint32_t imageHeight, RadeonRays::ray & currentRay, const PerspectiveCamera* cameraPtr) {


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
		RadeonRays::float2 cSample = hSample * cameraPtr->getSensorSize();


		// set ray
		//RadeonRays::ray& currentRay = renderData->host_rays[0][y * imageWidth + x];

		currentRay.d = RadeonRays::normalize(cameraPtr->getFocalLength() * cameraPtr->getForwardVector() + cSample.x * cameraPtr->getRightVector() + cSample.y * cameraPtr->getUpVector());
		currentRay.o = cameraPtr->getPosition() + cameraPtr->getDepthRange().x * currentRay.d;

		currentRay.o.w = cameraPtr->getDepthRange().y - cameraPtr->getDepthRange().x;
		currentRay.d.w = sampleBase.x;		// check
	}


}