
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>
#include <vector>

#include <iostream>

#include <string>

#include <mutex>
#include <shared_mutex>

#include "math/float3.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

// tmp
#include "../SP/src/Renderer/RenderOutput.hpp"

namespace SP {

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

		// for reading testing image only
		//ImageConfig(int localId, const std::string& path, Encoder* encoder, ImageBuffer& accBuffer);

		ImageConfig(int localId, const std::string& path);

		size_t getByteSize() const {
			return imageData.size() * sizeof(uint8_t);
		}

		const ImageBuffer& getImageData();


		// radiance 	
		const RadianceMap& getRadianceMap();


		void setRadiancePtr(std::shared_ptr<SP::RenderOutput> renderOut);

		int getID() const {
			return imageID;
		}

		void setId(int id) {
			imageID = id;
		}

		// write: unique_lock
		void setRefreshState(bool flag);

		// read: shared_lock 
		bool getRefreshState() const;


		// test code
		void storeToPPM(int serialNumber) const;

		// test code
		void storeToHDR(int serialNumber) const;

		// test code
		void reset();


		std::uint32_t getWidth() const;
		std::uint32_t getHeight() const;

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
		std::unique_ptr<boost::shared_mutex> flagMutexPtr{new boost::shared_mutex()};

		// test
		std::shared_ptr<SP::RenderOutput> radiancePtr;

	};




}






#endif