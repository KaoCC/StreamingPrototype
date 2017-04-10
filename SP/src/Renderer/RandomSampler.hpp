#ifndef _SP_RANDOMSAMPLER_HPP_
#define _SP_RANDOMSAMPLER_HPP_

#include "Sampler.hpp"

#include <memory>

namespace SP {

	class RandomSampler : public Sampler {

	public:

		static std::unique_ptr<RandomSampler> create(std::uint32_t seed);

		virtual float sample1D() override;
		virtual RadeonRays::float2 sample2D() override;


		RandomSampler(std::uint32_t seed);

	private:





		std::uint32_t mSeed;

	};






}










#endif

