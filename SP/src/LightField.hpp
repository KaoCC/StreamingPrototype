#ifndef _SP_LIGHTFIELD_HPP_
#define _SP_LIGHTFIELD_HPP_

#include "Common.hpp"

#include <vector>
#include <string>

namespace SP {

	class LightField {
	public:

		class SubLightField {

		public:
			SubLightField(size_t index, std::size_t numOfSubLFImgs);

			// test
			//SubLightField(size_t index, const std::string& basePath);

			const size_t mNumOfImgs;

			std::vector<ImageConfig> images;
			bool refreshFlag = false;
		};


		//number of Sub LFs, number of images
		LightField(std::size_t numOfSubLFs, std::size_t numOfSubLFImgs);

		ImageConfig::ImageBuffer getSubLightFieldImages(size_t subLFIdx);

		ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);

		//set
		void setSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx, const ImageConfig& imgConf);


		// refresh
		bool getSubLightFieldRefreshState(size_t subLFIdx) const;
		void setSubLightFieldRefreshState(size_t subLFIdx, bool state);


		std::size_t getTotalSize() const;
		std::size_t getSubLightFieldSize() const;

		// TEST !
		ImageConfig::ImageBuffer getAll();


	private:

		std::vector<SubLightField> subLFs;

		const std::size_t mNumOfSubLFs;
		const std::size_t mNumOfSubLFImgs;


	};









}









#endif

