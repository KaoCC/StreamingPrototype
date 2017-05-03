
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>
#include <vector>

#include <iostream>

#include <string>

#include "math/float3.h"

// tmp
#include "../SP/src/Renderer/RenderOutput.hpp"

namespace SP {


	//class Encoder;

	//struct Position {

	//	Position(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {
	//	}

	//	float x;
	//	float y;
	//	float z;
	//};

	//struct Direction {

	//	Direction(float vxIn, float vyIn, float vzIn) : vx(vxIn), vy(vyIn), vz(vzIn) {
	//	}

	//	float vx;
	//	float vy;
	//	float vz;
	//};

	struct CameraConfig {

		CameraConfig(RadeonRays::float3 pos, RadeonRays::float3 at, RadeonRays::float3 up) : mCameraPos(pos), mCameraAt(at), mCameraUp(up){
		}

		RadeonRays::float3 mCameraPos;
		RadeonRays::float3 mCameraAt;
		RadeonRays::float3 mCameraUp;
	};



	struct ScreenConfig {

		ScreenConfig(uint32_t w, uint32_t h) : width(w), height(h) {

		}

		std::uint32_t width;
		std::uint32_t height;
	};


	class ImageConfig {
	public:
		using ImageBuffer = std::vector<std::uint8_t>;
		using RadianceMap = std::vector<RadeonRays::float3>;

		ImageConfig() = default;

		// for testing only
		//ImageConfig(uint8_t init, size_t size) {
		//	imageData.resize(size);
		//	radiance.resize(size);

		//	for (size_t i = 0; i < size; ++i) {
		//		imageData[i] = init;
		//	}
		//}


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


		// radiance 	
		RadianceMap& getRadianceMap() {
			return radiance;
		}

		//KAOCC: test
		void setRadiancePtr(SP::RenderOutput* renderOut);

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

		// test code
		void storeToHDR(size_t serialNumber) const;

		// test code
		void reset();


		// ...

	private:


		// test
		int imageID;
		ImageBuffer imageData;

		// Renderer result cache
		RadianceMap radiance;


		// test
		SP::RenderOutput* radiancePtr;

	};




}






#endif