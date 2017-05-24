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


			ImageConfig& operator[] (std::size_t index);
			const ImageConfig& operator[] (std::size_t index) const;

			bool getRefreshState() const;
			//void setRefreshFlag(bool flag);

			size_t getNumOfImage() const;

		private:

			const size_t mNumOfImgs;

			std::vector<ImageConfig> images;
			//bool refreshFlag = false;
		};


		//number of Sub LFs, number of images
		LightField(std::size_t numOfSubLFs, std::size_t numOfSubLFImgs);

		//ImageConfig::ImageBuffer getSubLightFieldImages(size_t subLFIdx);

		//ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);

		//set
		//void setSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx, const ImageConfig& imgConf);
		//void setSubLightFieldRadianceWithIndex(size_t subLFIdx, size_t imgIdx, RenderOutput* renderOut);

		// refresh
		//bool getSubLightFieldRefreshState(size_t subLFIdx) const;
		//void setSubLightFieldRefreshState(size_t subLFIdx, bool state);


		// operators
		SubLightField& operator[] (std::size_t index);
		const SubLightField& operator[] (std::size_t index) const;


		std::size_t getTotalSize() const;
		std::size_t getSubLightFieldSize() const;

		// TEST !
		//ImageConfig::ImageBuffer getAll();

		// TEST
		void clearAll();

		// TEST
		void saveAll() const;

	private:

		std::vector<SubLightField> subLFs;

		const std::size_t mNumOfSubLFs;
		const std::size_t mNumOfSubLFImgs;


	};









}









#endif

