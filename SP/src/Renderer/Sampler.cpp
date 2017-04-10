
#include "Sampler.hpp"

#include "MathUtility.hpp"
#include "math/mathutils.h"

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
	//uint32_t UniformSampler_SampleUint(Sampler* sampler) {
	//	sampler->index = WangHash(1664525U * sampler->index + 1013904223U);
	//	return sampler->index;
	//}

	///// Return random float
	//float UniformSampler_Sample1D(Sampler* sampler) {
	//	return ((float)UniformSampler_SampleUint(sampler)) / 0xffffffffU;
	//}

	//RadeonRays::float2 Sampler_Sample2D(Sampler* sampler) {
	//	RadeonRays::float2 sample;
	//	sample.x = UniformSampler_Sample1D(sampler);
	//	sample.y = UniformSampler_Sample1D(sampler);
	//	return sample;

	//}

	//float Sampler_Sample1D(Sampler * sampler) {
	//	return UniformSampler_Sample1D(sampler);
	//}

	RadeonRays::float3 Sample_MapToHemisphere(RadeonRays::float2 sample, RadeonRays::float3 n, float e) {

		RadeonRays::float3 u = getOrthogonalVector(n);
		RadeonRays::float3 v = RadeonRays::cross(u, n);
		// calibration
		u = RadeonRays::cross(n, v);

		// Calculate 2D sample
		float r1 = sample.x;
		float r2 = sample.y;

		// Transform to spherical coordinates
		float sinpsi = std::sin(2 * PI * r1);
		float cospsi = std::cos(2 * PI * r1);
		float costheta = std::pow(1.f - r2, 1.f / (e + 1.f));
		float sintheta = std::sqrt(1.f - costheta * costheta);

		return RadeonRays::normalize(u * sintheta * cospsi + v * sintheta * sinpsi + n * costheta);

	}




}







