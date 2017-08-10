#include "LightField.hpp"
#include <iostream>

namespace SP {

	LightField::LightField(std::size_t numOfSubLFs, std::size_t numOfSubLFImgs) : mNumOfSubLFs(numOfSubLFs), mNumOfSubLFImgs(numOfSubLFImgs) {

		// test images
		//const std::string kFilePath{ pathBase + "crown_" + std::to_string(index) + ".ppm" };

		for (size_t i = 0; i < numOfSubLFs; ++i) {
			//subLFs.push_back(SubLightField(i, this->pathBase));
			subLFs.push_back(SubLightField(i, numOfSubLFImgs));
		}
	}

	LightField::SubLightField::SubLightField(size_t index, std::size_t numOfSubLFImgs) : mNumOfImgs(numOfSubLFImgs) {

		if (numOfSubLFImgs > 0) {
			images.resize(numOfSubLFImgs);
		}

	}

	ImageConfig & LightField::SubLightField::operator[](std::size_t index) {
		return images[index];
	}

	const ImageConfig & LightField::SubLightField::operator[](std::size_t index) const {
		return images[index];
	}

	bool LightField::SubLightField::getRefreshState() const {
		bool status = false;

		for (const auto &image : images) {
			if (image.getRefreshState()) {
				status = true;
				break;
			}
		}

		return status;
	}

	//void LightField::SubLightField::setRefreshFlag(bool flag) {
	//	refreshFlag = flag;
	//}

	size_t LightField::SubLightField::getNumOfImage() const {
		return mNumOfImgs;
	}


	// for testing
	//LightField::SubLightField::SubLightField(size_t index, const std::string & basePath) {

	//	std::cerr << "LOAD LF\n";

	//	for (size_t i = 0; i < numOfImgs; ++i) {
	//		//const std::string kFilePath{ basePath + "LF_crown_" + std::to_string(2 * index + i) + ".ppm" };
	//		const std::string kFilePath{ basePath + "LF_crown_" + std::to_string(index) + ".ppm" };
	//		images.push_back(ImageConfig(2 * index + i, kFilePath));
	//	}

	//	std::cerr << "LF FIN\n";

	//}

	//ImageConfig::ImageBuffer LightField::getSubLightFieldImages(size_t index) {

	//	ImageConfig::ImageBuffer buffer;

	//	for (size_t i = 0; i < subLFs[index].images.size(); ++i) {

	//		ImageConfig::ImageBuffer& imgBufferRef = subLFs[index].images[i].getImageData();
	//		buffer.insert(buffer.end(), imgBufferRef.begin(), imgBufferRef.end());

	//	}


	//	return buffer;
	//}

	//ImageConfig::ImageBuffer LightField::getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx) {
	//	return subLFs[subLFIdx].images[imgIdx].getImageData();
	//}

	//void LightField::setSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx, const ImageConfig & imgConf) {

	//	// KAOCC: WE NEED LOCK !

	//	subLFs[subLFIdx].images[imgIdx] = (imgConf);

	//	//std::cerr << "Write to sub LF: " << subLFIdx << " " << imgIdx << "\n";
	//}

	//void LightField::setSubLightFieldRadianceWithIndex(size_t subLFIdx, size_t imgIdx, RenderOutput * renderOut) {
	//	subLFs[subLFIdx].images[imgIdx].setRadiancePtr(renderOut);
	//}

	//bool LightField::getSubLightFieldRefreshState(size_t subLFIdx) const {
	//	return subLFs[subLFIdx].refreshFlag;
	//}

	//void LightField::setSubLightFieldRefreshState(size_t subLFIdx, bool state) {
	//	subLFs[subLFIdx].refreshFlag = state;
	//}

	LightField::SubLightField & LightField::operator[](std::size_t index) {
		return subLFs[index];
	}

	const LightField::SubLightField & LightField::operator[](std::size_t index) const {
		return subLFs[index];
	}

	size_t LightField::getTotalSize() const {
		return subLFs.size();
	}

	size_t LightField::getSubLightFieldSize() const {
		return mNumOfSubLFImgs;
	}

	// test only
	//ImageConfig::ImageBuffer LightField::getAll() {

	//	ImageConfig::ImageBuffer buffer;


	//	for (size_t i = 0; i < subLFs.size(); ++i) {

	//		for (size_t j = 0; j < subLFs[i].images.size(); ++j) {

	//			ImageConfig::ImageBuffer& imgBufferRef = subLFs[i].images[j].getImageData();
	//			buffer.insert(buffer.end(), imgBufferRef.begin(), imgBufferRef.end());
	//			buffer.insert(buffer.end(), imgBufferRef.begin(), imgBufferRef.end());

	//		}


	//	}

	//	return buffer;
	//}

	void LightField::clearAll() {

		for (auto& subLF : subLFs) {
			for (size_t i = 0; i < subLF.getNumOfImage(); ++i) {
				subLF[i].reset();
			}

		}

	}

	void LightField::saveAll() const {

		for (const auto& subLF : subLFs) {
			for (size_t i = 0; i < subLF.getNumOfImage(); ++i) {
				subLF[i].storeToPPM(-1);
				subLF[i].storeToHDR(-1);
			}
		}

	}


	// test




}
