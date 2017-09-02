#ifndef _SP_TEXTURE_HPP_
#define _SP_TEXTURE_HPP_

#include "math/int2.h"

#include "SceneObject.hpp"

#include "math/float3.h"

#include <vector>

namespace SP {



	class Texture : public SceneObject {


	public:
		enum class Format {
			kRGBA8,
			kRGBA16,
			kRGBA32
		};


		Texture(std::vector<std::uint8_t> vec, RadeonRays::int2 size, Format format);
		virtual ~Texture() = default;


		RadeonRays::float3 sampleEnvMap(RadeonRays::float3 d);

		RadeonRays::float4 sample2D(RadeonRays::float2 uv) const;

		// Disallow copying
		Texture(Texture const&) = delete;
		Texture& operator = (Texture const&) = delete;

	private:

		// data
		//std::unique_ptr<const std::uint8_t[]> data;

		std::vector<std::uint8_t> texData;

		// size
		RadeonRays::int2 size;
		//format
		Format format;

	};




}











#endif

