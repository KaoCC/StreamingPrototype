#include "Texture.hpp"


namespace SP {

	Texture::Texture(const std::uint8_t* ptr, RadeonRays::int2 sz, Format fmt) : data(ptr), size(sz), format(fmt) {
	}

}
