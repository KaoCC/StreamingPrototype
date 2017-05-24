
#include <algorithm>
#include <fstream>	// for reading test images
#include <iterator>

#include <iostream>

#include <string>


#include "Common.hpp"
#include "Encoder/Encoder.hpp"

#include "math/mathutils.h"

#include <OpenImageIO/imageio.h>


namespace SP {


	//ImageConfig::ImageConfig(int localId, const std::string& path, Encoder* encoder, ImageBuffer& accBuffer) {
	ImageConfig::ImageConfig(int localId, const std::string& path) {

		imageID = localId;

		std::ifstream inputStream(path, std::ifstream::binary);

		// temp, should fix this
		if (!inputStream) {
			throw std::runtime_error("Could not open file");;
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
		std::copy(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>(), std::back_inserter(imageData));
		//} catch (const std::ios_base::failure &fail) {
		//std::cerr << fail.what() << '\n';
		//}


		// Compress here

		//Encoder* encoder = CreateEncoder(imageWidth, imageHeight);

		//uint8_t* rawPtr = encoder->getEncoderRawBuffer();


		//std::copy(imageData.begin(), imageData.end(), rawPtr);

		//uint8_t* outBufPtr;
		//int outSize = 0;
		//encoder->startEncoding(&outBufPtr, &outSize);


		//if (outSize > 0) {

		//	std::cerr << "SUCCESS! " << outSize << '\n';

		//	//ImageBuffer encodedImageData(outBufPtr, outBufPtr + outSize);

		//	//tmp
		//	imageData = std::move(ImageBuffer(outBufPtr, outBufPtr + outSize));


		//	// accumulate
		//	accBuffer.insert(std::end(accBuffer), std::begin(imageData), std::end(imageData));



		//}

	}

	void ImageConfig::setRadiancePtr(SP::RenderOutput * renderOut) {
		radiancePtr = renderOut;
	}

	void ImageConfig::storeToPPM(size_t serialNumber) const {

		if (serialNumber == -1) {
			serialNumber = this->imageID;
		}

		const std::string fileName = "image" + std::to_string(imageID) + "-" + std::to_string(serialNumber) +".ppm";

		FILE* file = fopen(fileName.c_str(), "wb");
		if (!file) {
			//throw std::runtime_error("cannot open file " + fileName.str());

			std::cerr << "Error !\n";
		}

		// tmp
		fprintf(file, "P6\n%i %i\n255\n", getWidth(), getHeight());

		for (size_t i = 0; i < imageData.size(); ++i) {
			fputc(imageData[i], file);
		}

		fclose(file);


	}

	// test
	void ImageConfig::storeToHDR(size_t serialNumber) const {
		OIIO_NAMESPACE_USING

		if (serialNumber == -1) {
			serialNumber = this->imageID;
		}

		const std::string fileName = "radiance" + std::to_string(imageID) + "-" + std::to_string(serialNumber) + ".hdr";

		// KAOCC: TMP !!!!!!!!!!!!
		const int xres = getWidth();
		const int yres = getHeight();
		const int channels = 3; // RGB
		//unsigned char pixels[xres*yres*channels];

		ImageOutput* imgOut = ImageOutput::create(fileName);

		if (!imgOut) {
			throw std::runtime_error("Failed to create image: " + fileName);
		}

		ImageSpec spec(xres, yres, channels, TypeDesc::FLOAT);
		imgOut->open(fileName, spec);
		imgOut->write_image(TypeDesc::FLOAT, radiance.data(), sizeof(RadeonRays::float3));
		imgOut->close();

		delete imgOut;
	}

	void ImageConfig::reset() {

		// reset all data to 0
		for (auto& data : imageData) {
			data = 0;
		}
	}


}


