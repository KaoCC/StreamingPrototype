
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>
#include <vector>

#include <algorithm>
#include <fstream>	// for reading test images
#include <iterator>

#include <iostream>

namespace SP {


	struct Position {

		Position(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {
		}

		float x;
		float y;
		float z;
	};

	struct Direction {

		Direction(float vxIn, float vyIn, float vzIn) : vx(vxIn), vy(vyIn), vz(vzIn) {
		}

		float vx;
		float vy;
		float vz;
	};

	struct CameraConfig {

		CameraConfig(Position p, Direction d) : pos(p), dir(d) {
		}

		Position pos;
		Direction dir;
	};



	struct ScreenConfig {

		ScreenConfig(uint32_t w, uint32_t h) : width(w), height(h) {

		}

		uint32_t width;
		uint32_t height;
	};


	class ImageConfig {
	public:
		using ImageBuffer = std::vector<uint8_t>;

		ImageConfig() = default;

		// for testing only
		ImageConfig(uint8_t init, size_t size) {
			imageData.resize(size);

			for (size_t i = 0; i < size; ++i) {
				imageData[i] = init;
			}
		}


		// for reading testing image only
		explicit ImageConfig(const std::string& path) {

			std::ifstream inputStream(path, std::ifstream::in | std::ifstream::binary);

			// temp, should fix this
			if (!inputStream) {
				throw "Error";
			}


			// check this
			inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				std::copy(std::istream_iterator<std::uint8_t>(inputStream), std::istream_iterator<std::uint8_t>(), std::back_inserter(imageData));
			} catch (const std::ios_base::failure &fail) {
				std::cerr << fail.what() << '\n';
			}
		}

		size_t getByteSize() const {
			return imageData.size() * sizeof(uint8_t);
		}


		ImageBuffer& getImageData() {
			return imageData;
		}

		const ImageBuffer& getImageData() const {
			return imageData;
		}

		const uint8_t* getImageRawData() const {
			return imageData.data();
		}

		uint8_t* getImageRawData() {
			return imageData.data();
		}

		// ...

	private:

		ImageBuffer imageData;
	};




}






#endif