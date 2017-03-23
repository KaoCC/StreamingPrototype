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


		private:


			// test
			const size_t numOfImgs = 2;

			std::vector<ImageConfig> images;

			// tmp file location
			const std::string pathBase{ "../Resources/SceneImages/" };
		};


		LightField();


	private:

		std::vector<SubLightField> subLFs;

		// test
		const size_t numOfSubLFs = 4;

	};









}









#endif

