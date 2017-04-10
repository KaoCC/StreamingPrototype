#include "CorrelatedMultiJitterSampler.hpp"

namespace SP {

	const std::uint32_t CorrelatedMultiJitterSampler::kDim = 16;

	std::unique_ptr<CorrelatedMultiJitterSampler> CorrelatedMultiJitterSampler::create(std::uint32_t index, std::uint32_t dimension, std::uint32_t scramble) {
		return std::make_unique<CorrelatedMultiJitterSampler>(index, dimension, scramble);
	}

	CorrelatedMultiJitterSampler::CorrelatedMultiJitterSampler(std::uint32_t index, std::uint32_t dimension, std::uint32_t scramble):
		mIndex(index), mDimension(dimension), mScramble(scramble) {
	}

	float CorrelatedMultiJitterSampler::sample1D() {
		int idx = permute(mIndex, kDim * kDim, 0xa399d265 * mDimension * mScramble);
		++mDimension;

		return cmj(idx, kDim, mDimension * mScramble).x;
	}

	RadeonRays::float2 CorrelatedMultiJitterSampler::sample2D() {
		int idx = permute(mIndex, kDim * kDim, 0xa399d265 * mDimension * mScramble);
		++mDimension;

		return cmj(idx, kDim, mDimension * mScramble);
	}


}
