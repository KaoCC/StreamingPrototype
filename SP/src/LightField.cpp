#include "LightField.hpp"


namespace SP {

	LightField::LightField() {

		// test images
		//const std::string kFilePath{ pathBase + "crown_" + std::to_string(index) + ".ppm" };

		for (size_t i = 0; i < numOfSubLFs; ++i) {
			subLFs.push_back(SubLightField(i));
		}
	}

	LightField::SubLightField::SubLightField(size_t index) {


		for (size_t i = 0; i < numOfImgs; ++i) {
			const std::string kFilePath{ pathBase + "LF_crown_" + std::to_string(2 * index+ i) + ".ppm" };
			images.push_back(ImageConfig(0, kFilePath));
		}


	}

}
