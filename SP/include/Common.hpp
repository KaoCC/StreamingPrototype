
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>
#include <vector>

#include <iostream>

#include <string>

#include <mutex>

#include "math/mathutils.h"
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


		//ImageBuffer& getImageData() {
		//	return imageData;
		//}

		const ImageBuffer& getImageData() {

			// get Radiance Map
			// Note: will check the refresh flag
			const auto& tmpRadiance = getRadianceMap();

			if (cacheFlag) {

				// convert
				const size_t kStride = 3;

				const size_t screenWidth = getWidth();
				const size_t screenHeight = getHeight();

				imageData.resize(tmpRadiance.size() * kStride);

				// tmp gamma
				const float gamma = 2.2f;

				//for (size_t i = 0; i < fdata.size(); ++i) {
				//	imgBufferRef[kStride * i] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].x / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
				//	imgBufferRef[kStride * i + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].y / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
				//	imgBufferRef[kStride * i + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(fdata[i].z / fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
				//	//imgBufferRef[kStride * i + 3] = 1;
				//}

				size_t currentindex = 0;

				for (size_t y = 0; y < screenHeight; ++y) {
					for (size_t x = 0; x < screenWidth; ++x) {

						const RadeonRays::float3& val = tmpRadiance[(screenHeight - 1 - y) * screenWidth + x];

						imageData[currentindex] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.x / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
						imageData[currentindex + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.y / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
						imageData[currentindex + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.z / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));

						currentindex += kStride;
					}

				}

				cacheFlag = false;
			}

			return imageData;
		}


		// radiance 	
		const RadianceMap& getRadianceMap()  {

			if (getRefreshState()) {
				// save the Radiance
				radiance = radiancePtr->copyData();

				cacheFlag = true;
				setRefreshState(false);
			}

			return radiance;
		}

		

		//KAOCC: test
		void setRadiancePtr(SP::RenderOutput* renderOut);

		//const uint8_t* getImageRawData() const {
		//	return imageData.data();
		//}

		//uint8_t* getImageRawData() {
		//	return imageData.data();
		//}

		int getID() const {
			return imageID;
		}

		void setId(int id) {
			imageID = id;
		}


		// lock ?
		void setRefreshState(bool flag) {
			std::lock_guard<std::mutex> flagLock(*flagMutexPtr);
			refreshFlag = flag;
		}

		// lock ?
		bool getRefreshState() {

			bool tmpState = false;

			{
				std::lock_guard<std::mutex> flagLock(*flagMutexPtr);
				tmpState = refreshFlag;
			}

			return tmpState;
		}


		// test code
		void storeToPPM(size_t serialNumber) const;

		// test code
		void storeToHDR(size_t serialNumber) const;

		// test code
		void reset();


		std::uint32_t getWidth() const {
			if (radiancePtr != nullptr) {
				return radiancePtr->getWidth();
			} else {
				return 0;
			}
		}

		std::uint32_t getHeight() const {
			if (radiancePtr != nullptr) {
				return radiancePtr->getHeight();
			} else {
				return 0;
			}
		}

		// ...

	private:


		// test
		int imageID;
		ImageBuffer imageData;

		// Renderer result cache
		RadianceMap radiance;

		//need lock ?
		volatile bool refreshFlag = false;
		
		bool cacheFlag = false;

		//workaround
		std::unique_ptr<std::mutex> flagMutexPtr{new std::mutex()};

		// test
		SP::RenderOutput* radiancePtr = nullptr;

	};




}






#endif