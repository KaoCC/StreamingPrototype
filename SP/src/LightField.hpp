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
			const size_t numOfImgs = 2;

			std::vector<ImageConfig> images;

			// tmp file location
			const std::string pathBase{ "../Resources/LF/" };
		};


		LightField();

		ImageConfig::ImageBuffer getSubLightFieldImages(size_t index);

		// TEST !
		ImageConfig::ImageBuffer getAll();


	private:

		std::vector<SubLightField> subLFs;

		// test
		const size_t numOfSubLFs = 3;

	};









}









#endif

