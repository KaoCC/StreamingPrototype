#ifndef _SP_CORRELATEDMULTIJITTERSAMPLER_HPP_
#define _SP_CORRELATEDMULTIJITTERSAMPLER_HPP_

#include "Sampler.hpp"

#include <memory>

namespace SP {

	class CorrelatedMultiJitterSampler : public Sampler {


	public:

		static std::unique_ptr<CorrelatedMultiJitterSampler> create(std::uint32_t index, std::uint32_t  dimension, std::uint32_t  scramble);

		CorrelatedMultiJitterSampler(std::uint32_t index, std::uint32_t  dimension, std::uint32_t  scramble);


		// Inherited via Sampler
		virtual float sample1D() override;

		virtual RadeonRays::float2 sample2D() override;

		static const std::uint32_t kDim;

	private:


		unsigned permute(unsigned i, unsigned l, unsigned p) {
			unsigned w = l - 1;
			w |= w >> 1;
			w |= w >> 2;
			w |= w >> 4;
			w |= w >> 8;
			w |= w >> 16;

			do {
				i ^= p;
				i *= 0xe170893d;
				i ^= p >> 16;
				i ^= (i & w) >> 4;
				i ^= p >> 8;
				i *= 0x0929eb3f;
				i ^= p >> 23;
				i ^= (i & w) >> 1;
				i *= 1 | p >> 27;
				i *= 0x6935fa69;
				i ^= (i & w) >> 11;
				i *= 0x74dcb303;
				i ^= (i & w) >> 2;
				i *= 0x9e501cc3;
				i ^= (i & w) >> 2;
				i *= 0xc860a3df;
				i &= w;
				i ^= i >> 5;
			} while (i >= l);
			return (i + p) % l;
		}

		float randfloat(unsigned i, unsigned p) {
			i ^= p;
			i ^= i >> 17;
			i ^= i >> 10;
			i *= 0xb36534e5;
			i ^= i >> 12;
			i ^= i >> 21;
			i *= 0x93fc4795;
			i ^= 0xdf6e307f;
			i ^= i >> 17;
			i *= 1 | p >> 18;
			return i * (1.0f / 4294967808.0f);
		}

		// helper
		RadeonRays::float2 cmj(int s, int n, int p) {
			int sx = permute(s % n, n, p * 0xa511e9b3);
			int sy = permute(s / n, n, p * 0x63d83595);
			float jx = randfloat(s, p * 0xa399d265);
			float jy = randfloat(s, p * 0x711ad6a5);

			return RadeonRays::float2((s % n + (sy + jx) / n) / n,
				(s / n + (sx + jy) / n) / n);
		}



		std::uint32_t mIndex;
		std::uint32_t mDimension;
		std::uint32_t mScramble;



	};








}











#endif

