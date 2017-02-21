#ifndef _SP_TEXTURE_HPP_
#define _SP_TEXTURE_HPP_

#include "math/int2.h"

#include "SceneObject.hpp"

#include <memory>

namespace SP {



	class Texture : public SceneObject {


	public:
		enum class Format {
			kRGBA8,
			kRGBA16,
			kRGBA32
		};


		Texture(const std::uint8_t* ptr, RadeonRays::int2 size, Format format);
		virtual ~Texture() = default;


		// Disallow copying
		Texture(Texture const&) = delete;
		Texture& operator = (Texture const&) = delete;

	private:

		// data
		std::unique_ptr<const std::uint8_t[]> data;
		// size
		RadeonRays::int2 size;
		//format
		Format format;

	};




}











#endif

