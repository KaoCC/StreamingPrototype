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
			SubLightField(size_t index);

			// test
			SubLightField(size_t index, const std::string& basePath);


			// test
			const size_t numOfImgs = 2;

			std::vector<ImageConfig> images;

			bool refreshFlag = false;

		};


		LightField();

		ImageConfig::ImageBuffer getSubLightFieldImages(size_t subLFIdx);

		ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);

		//setter
		void setSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx, const ImageConfig& imgConf);


		// refresh
		bool getSubLightFieldRefreshState(size_t subLFIdx) const;
		void setSubLightFieldRefreshState(size_t subLFIdx, bool state);


		size_t getTotalSize() const;
		size_t getSubLightFieldSize(size_t subLFIdx) const;

		// TEST !
		ImageConfig::ImageBuffer getAll();


	private:

		std::vector<SubLightField> subLFs;


		// test
		const size_t numOfSubLFs = 8;


		// tmp file location
		const std::string pathBase{ "../Resources/LF/" };


	};









}









#endif

