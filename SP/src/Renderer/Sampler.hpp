#ifndef _SP_SAMPLER_HPP_
#define _SP_SAMPLER_HPP_

#include <cstdint>

// TMP code for sampler here !

// tmp code

namespace SP {

	struct Sampler {
		uint32_t index;
		uint32_t dimension;
		uint32_t scramble;
	};


	uint32_t WangHash(uint32_t seed);
	uint32_t UniformSampler_SampleUint(Sampler* sampler);
	float UniformSampler_Sample1D(Sampler* sampler);
}














#endif

