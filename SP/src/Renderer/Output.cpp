#include "Output.hpp"

namespace SP {

	Output::Output(std::size_t w, std::size_t h) : width{ w }, height{ h } {
	}

	std::size_t Output::getWidth() const {
		return width;
	}

	std::size_t Output::getHeight() const {
		return height;
	}

	std::size_t Output::getSize() const {
		return getStorageSize();
	}

	void Output::resize(std::size_t w, std::size_t h) {
		width = w;
		height = h;
		resizeStorage(w * h);
	}

	void Output::resetToDefault() {
		resetStorageDefault();
	}

}
