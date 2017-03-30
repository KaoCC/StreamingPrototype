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
			const size_t numOfImgs = 8;

			std::vector<ImageConfig> images;

		};


		LightField();

		ImageConfig::ImageBuffer getSubLightFieldImages(size_t subLFIdx);

		ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);


		size_t getTotalSize() const;
		size_t getSubLightFieldSize(size_t subLFIdx) const;

		// TEST !
		ImageConfig::ImageBuffer getAll();


	private:

		std::vector<SubLightField> subLFs;

		// test
		const size_t numOfSubLFs = 16;


		// tmp file location
		const std::string pathBase{ "../Resources/LF/" };

	};









}









#endif

