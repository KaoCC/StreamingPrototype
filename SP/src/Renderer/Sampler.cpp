
#include "Sampler.hpp"


namespace SP {

	/**
	Random sampler
	**/

	// Hash function
	uint32_t WangHash(uint32_t seed) {
		seed = (seed ^ 61) ^ (seed >> 16);
		seed *= 9;
		seed = seed ^ (seed >> 4);
		seed *= 0x27d4eb2d;
		seed = seed ^ (seed >> 15);
		return seed;
	}

	/// Return random unsigned
	uint32_t UniformSampler_SampleUint(Sampler* sampler) {
		sampler->index = WangHash(1664525U * sampler->index + 1013904223U);
		return sampler->index;
	}

	/// Return random float
	float UniformSampler_Sample1D(Sampler* sampler) {
		return ((float)UniformSampler_SampleUint(sampler)) / 0xffffffffU;
	}


	// End of tmp code for sampler

}







