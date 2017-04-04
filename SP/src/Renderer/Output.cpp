#include "Output.hpp"

namespace SP {

	Output::Output(std::uint32_t w, std::uint32_t h) : width(w), height(h) {
	}

	std::uint32_t Output::getWidth() const {
		return width;
	}

	std::uint32_t Output::getHeight() const {
		return height;
	}

}
