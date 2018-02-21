
#include <algorithm>
#include <fstream>	// for reading test images
#include <iterator>

#include <iostream>

#include <string>


#include "Common.hpp"
#include "Encoder/Encoder.hpp"

#include "math/mathutils.h"

#include <OpenImageIO/imageio.h>

#include <boost/filesystem.hpp>

namespace SP {

	// helper function for adding path
	static std::string addPrefixPath(const std::string& fileName) {

		const boost::filesystem::path kDefaultConfigFile { "folder_config.txt" };

		// current path
		//const std::string kDefaultPrefix = "./";


		if (!boost::filesystem::exists(kDefaultConfigFile)) {
			std::cerr << "folder_config.txt does not exist, use default config\n";
			return boost::filesystem::current_path().append(fileName).generic_string();
		}

		// read default config file for folder location
		std::ifstream inputConfig { kDefaultConfigFile.generic_string() };

		if (!inputConfig) {
			throw std::runtime_error("Could not open file");
		}

		// check this
		inputConfig.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string folderPath;
		std::getline(inputConfig, folderPath);


		// Create the folder if not exist
		if (!boost::filesystem::exists(folderPath)) {
			std::cerr << "Folder not exist, creating ...\n";
			
			if (!boost::filesystem::create_directories(folderPath)) {
				std::cerr << "Fail to create dir, use dafault\n";
				return boost::filesystem::current_path().append(fileName).generic_string();
			}

		} else if (!boost::filesystem::is_directory(folderPath)) {
			// not a folder, use default
			std::cerr << "Not a folder, use default path\n";
			return boost::filesystem::current_path().append(fileName).generic_string();
		}

		return boost::filesystem::path(folderPath).append(fileName).generic_string();

	}


	//ImageConfig::ImageConfig(int localId, const std::string& path, Encoder* encoder, ImageBuffer& accBuffer) {
	ImageConfig::ImageConfig(int localId, const std::string& path) {

		imageID = localId;

		std::ifstream inputStream(path, std::ifstream::binary);

		// temp, should fix this
		if (!inputStream) {
			throw std::runtime_error("Could not open file");
		}


		// check this
		inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		// KAOCC: test with ppm image!

		std::string magicNum;
		size_t imageWidth = 0;
		size_t imageHeight = 0;
		int maxColVal;

		std::getline(inputStream, magicNum);
		inputStream >> imageWidth >> imageHeight >> maxColVal;
		inputStream.get(); // skip the trailing white space

		std::cerr << "Image: " << magicNum << " " << imageWidth << " " << imageHeight << std::endl;


		// Read RGB buffer

		//try {
		std::copy(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>(), std::back_inserter(imageDataCache));
		//} catch (const std::ios_base::failure &fail) {
		//std::cerr << fail.what() << '\n';
		//}

	}

	ImageConfig::ImageConfig(const ImageConfig & rhs) : imageID{ rhs.imageID }, imageDataCache{ rhs.imageDataCache }, radiance{ rhs.radiance },
		refreshFlag{ rhs.refreshFlag.load() }, refreshCacheFlag{ rhs.refreshCacheFlag }, radiancePtr{ rhs.radiancePtr } {
	}

	ImageConfig & ImageConfig::operator=(const ImageConfig & rhs) {
		imageID = rhs.imageID;
		imageDataCache = rhs.imageDataCache;
		radiance = rhs.radiance;
		refreshFlag = rhs.refreshFlag.load();
		refreshCacheFlag = rhs.refreshCacheFlag;
		radiancePtr = rhs.radiancePtr;

	}

	const ImageConfig::ImageBuffer & ImageConfig::getImageCacheData() {
		// get Radiance Map
		// Note: will check the refresh flag
		const auto& tmpRadiance = getRadianceMap();

		if (refreshCacheFlag) {

			// convert
			const size_t kStride = 3;

			const size_t screenWidth = getWidth();
			const size_t screenHeight = getHeight();

			imageDataCache.resize(tmpRadiance.size() * kStride);

			// tmp gamma
			const float gamma = 2.2f;

			size_t currentindex = 0;

			for (size_t y = 0; y < screenHeight; ++y) {
				for (size_t x = 0; x < screenWidth; ++x) {

					const RadeonRays::float3& val = tmpRadiance[(screenHeight - 1 - y) * screenWidth + x];

					imageDataCache[currentindex] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.x / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
					imageDataCache[currentindex + 1] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.y / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));
					imageDataCache[currentindex + 2] = static_cast<uint8_t>(RadeonRays::clamp(RadeonRays::clamp(pow(val.z / val.w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255));

					currentindex += kStride;
				}

			}

			refreshCacheFlag = false;
		}

		return imageDataCache;
	}

	const ImageConfig::RadianceMap & ImageConfig::getRadianceMap() {
		if (getRefreshState()) {
			// save the Radiance
			radiance = radiancePtr->copyData();

			refreshCacheFlag = true;
			setRefreshState(false);
		}

		return radiance;
	}

	void ImageConfig::setRadiancePtr(std::shared_ptr<SP::RenderOutput> renderOut) {
		radiancePtr = renderOut;
	}

	void ImageConfig::setRefreshState(bool flag) {
		refreshFlag = flag;
	}

	bool ImageConfig::getRefreshState() const {
		return refreshFlag;
	}

	void ImageConfig::storeToPPM() {


		int serialNumber = this->imageID;


		std::string fileName = "image" + std::to_string(imageID) + "-" + std::to_string(serialNumber) +".ppm";
		fileName = addPrefixPath(fileName);

		FILE* file = fopen(fileName.c_str(), "wb");
		if (!file) {
			//throw std::runtime_error("cannot open file " + fileName.str());

			std::cerr << "Error: cannot open ppm file !\n";
		}

		// refresh the cache data
		const auto& cacheData = getImageCacheData();

		// tmp
		std::fprintf(file, "P6\n%i %i\n255\n", getWidth(), getHeight());


		for (size_t i = 0; i < cacheData.size(); ++i) {
			fputc(cacheData[i], file);
		}

		fclose(file);


	}

	// test
	void ImageConfig::storeToHDR() const {
		OIIO_NAMESPACE_USING


		int serialNumber = this->imageID;

		std::string fileName = "radiance" + std::to_string(imageID) + "-" + std::to_string(serialNumber) + ".exr";
		fileName = addPrefixPath(fileName);

		std::cerr << "FileName: " << fileName << '\n';

		const auto xres = getWidth();
		const auto yres = getHeight();
		const int channels = 3; // RGB

		ImageOutput* imgOut{ ImageOutput::create(fileName) };

		if (!imgOut) {
			throw std::runtime_error("Failed to create image: " + fileName);
		}

		const auto totalNum = xres * yres;
		const std::size_t tmpSz = totalNum * channels;

		//tmp, may need lock
		const auto& rr = radiancePtr->copyData();

		float* tmp = new float[tmpSz];

		for (int i = 0; i < rr.size(); ++i) {
			tmp[i * channels] = rr[i].x;
			tmp[i * channels + 1] = rr[i].y;
			tmp[i * channels + 2] = rr[i].z;
		}

		ImageSpec spec(xres, yres, channels, TypeDesc::FLOAT);
		spec.attribute("gammarank", rr[0].w);		// test

		imgOut->open(fileName, spec);
		imgOut->write_image(TypeDesc::FLOAT, tmp);
		imgOut->close();

		delete imgOut;
		delete [] tmp;
	}

	// tmp
	void ImageConfig::reset() {

		// reset all data to 0
		for (auto& data : imageDataCache) {
			data = 0;
		}
	}

	std::size_t ImageConfig::getWidth() const {
		if (radiancePtr != nullptr) {
			return radiancePtr->getWidth();
		}

        return 0;

    }

	std::size_t ImageConfig::getHeight() const {
		if (radiancePtr != nullptr) {
			return radiancePtr->getHeight();
		}

        return 0;

    }


}


