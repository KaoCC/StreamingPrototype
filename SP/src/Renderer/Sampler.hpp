#ifndef _SP_SAMPLER_HPP_
#define _SP_SAMPLER_HPP_

#include <cstdint>

// TMP code for sampler here !

// tmp code

#include "math/float2.h"
#include "math/float3.h"

namespace SP {

	struct Sampler {
		uint32_t index;
		uint32_t dimension;
		uint32_t scramble;
	};


	uint32_t WangHash(uint32_t seed);
	uint32_t UniformSampler_SampleUint(Sampler* sampler);
	float UniformSampler_Sample1D(Sampler* sampler);



	RadeonRays::float2 Sampler_Sample2D(Sampler* sampler);

	float Sampler_Sample1D(Sampler* sampler);


	RadeonRays::float3 Sample_MapToHemisphere(
		// Sample
		RadeonRays::float2 sample,
		// Hemisphere normal
		RadeonRays::float3 n,
		// Cos power
		float e
	);


}














#endif

