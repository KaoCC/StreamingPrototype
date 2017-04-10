#include "RandomSampler.hpp"

#include <memory>

namespace SP {
	std::unique_ptr<RandomSampler> RandomSampler::create(std::uint32_t seed) {

		std::unique_ptr<RandomSampler> samplerInst = std::make_unique<RandomSampler>(seed);
		return std::move(samplerInst);
	}

	float RandomSampler::sample1D() {
		mSeed = WangHash(1664525U * mSeed + 1013904223U);
		return static_cast<float>(mSeed) / 0xffffffffU;
	}

	RadeonRays::float2 RandomSampler::sample2D() {
		RadeonRays::float2 result;
		result.x = sample1D();
		result.y = sample1D();

		return result;
	}

	RandomSampler::RandomSampler(std::uint32_t seed) : mSeed(seed) {
	}

}
