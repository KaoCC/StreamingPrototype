
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>
#include <vector>

#include <iostream>

#include <string>

namespace SP {


	class Encoder;

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
		//ImageConfig(int localId, const std::string& path, Encoder* encoder, ImageBuffer& accBuffer);

		ImageConfig(int localId, const std::string& path);

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

		int getID() const {
			return imageID;
		}

		void setId(int id) {
			imageID = id;
		}


		// test code
		void storeToPPM(size_t serialNumber) const;


		// ...

	private:


		// test
		int imageID;
		ImageBuffer imageData;
	};




}






#endif