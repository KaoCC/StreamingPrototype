#ifndef _SP_CONFIGMANAGER_HPP_
#define _SP_CONFIGMANAGER_HPP_

#include "Common.hpp"

#include <vector>

#include "SyncBuffer.hpp"
#include "Encoder/Encoder.hpp"
#include "LightField.hpp"

namespace SP {

	class ConfigManager {


	public:
		//ConfigManager(SyncBuffer<ImageConfig>& buffer, LightField& imgLF);

		ConfigManager();

		//CameraConfig getCamera();

		//void setScreen(std::uint32_t w, std::uint32_t h);
		//ScreenConfig getScreen();

		void setModuleID(std::uint32_t mID);

		//void setPositionDelta(float dx, float dy, float dz);

		//const ImageConfig& getImageRef(size_t index);

		//ImageConfig getImage();
		//ImageConfig getImageCache();


		// LF indexing
		std::size_t getIndexOfSubLightField(float dx) const;


		// Light Field
		ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);


		// tmp
		bool getSubLightFieldRefreshState(std::size_t subLFIdx) const;
		void setSubLightFieldRefreshState(std::size_t subLFIdx, bool state);


		const LightField& getLightField() const;
		LightField& getLightField();


		std::size_t getNumberOfSubLFs() const;
		std::size_t getNumberOfSubLFImages() const;

		// Screen Size
		std::size_t getScreenWidth() const;
		std::size_t getScreenHeight() const;


		// Camera
		std::size_t getNumberOfCameras() const;

		// tmp
		//void loadImages();

	private:

		uint32_t moduleID = 0;

		//CameraConfig cameraCfg;
		//ScreenConfig screenCfg;

		// could be used as a cache, but we disable it right now
		//std::vector<ImageConfig> images;

		// cache for current image
		//SyncBuffer<ImageConfig>::DataPointer imagePtr{ new ImageConfig };

		// timer linit, wait no longer than this
		const int kTimeLimit = 10;

		//SyncBuffer<ImageConfig>& bufferRef;

		LightField mImageLightField;




		// Defaults !

		// Screen
		static const std::size_t kWidth = 512;
		static const std::size_t kHeight = 512;

		// LFs
		static const std::size_t kNumOfLFs = 4;
		static const std::size_t kNumOfSubLFImgs = 4;


		// A set: one camera, one render, one output, one thread
		static const size_t kNumOfCamera = kNumOfLFs * kNumOfSubLFImgs;



	};



}












#endif









