#ifndef _SP_CONFIGMANAGER_HPP_
#define _SP_CONFIGMANAGER_HPP_

#include "Common.hpp"

#include <vector>

#include "SyncBuffer.hpp"
#include "Encoder/Encoder.hpp"
#include "LightField.hpp"


#include "math/float3.h"

namespace SP {

	class RenderingManager;

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

		// LF multi index
		std::vector<std::size_t> getIndexArrayOfSubLightField(float dx) const;

		// Light Field
		//ImageConfig::ImageBuffer getSubLightFieldImageWithIndex(size_t subLFIdx, size_t imgIdx);

		// LF control
		void clearAll();
		void saveAll();

		// Render change Scene flag (control)
		bool isSceneChanged(size_t index) const;
		void setSceneChangedFlag(size_t index, bool flag);
		void setAllSceneChangedFlag(bool flag);		// tmp


		const LightField& getLightField() const;
		LightField& getLightField();


		std::size_t getNumberOfSubLFs() const;
		std::size_t getNumberOfSubLFImages() const;

		// Screen Size
		std::size_t getScreenWidth() const;
		std::size_t getScreenHeight() const;


		// Camera
		std::size_t getNumberOfCameras() const;

		CameraConfig getCameraConfig() const;

		// tmp
		//void loadImages();


		// Writebuffer Size
		std::size_t getWriteBufferSize() const;


		// workaround
		void setRenderManagerPtr(RenderingManager* renManPtr);

		void resetRenderer();

	private:

		uint32_t moduleID = 0;

		//CameraConfig cameraCfg;
		//ScreenConfig screenCfg;

		// could be used as a cache, but we disable it right now
		//std::vector<ImageConfig> images;

		// cache for current image
		//SyncBuffer<ImageConfig>::DataPointer imagePtr{ new ImageConfig };

		// timer limit, wait no longer than this
		const int kTimeLimit = 10;

		//SyncBuffer<ImageConfig>& bufferRef;

		LightField mImageLightField;

		CameraConfig mCamera;


		// Defaults !

		// Screen
		static const std::size_t kWidth = 256;
		static const std::size_t kHeight = 256;

		// LFs
		static const std::size_t kNumOfLFs = 8;
		static const std::size_t kNumOfSubLFImgs = 8;


		// A set: one camera, one render, one output, one thread
		static const size_t kNumOfCamera = kNumOfLFs * kNumOfSubLFImgs;


		// LF camera default pos
		static const RadeonRays::float3 kCameraPos; // = RadeonRays::float3(0.f, 1.f, 3.f);
		static const RadeonRays::float3 kCameraAt; // = RadeonRays::float3(0.f, 1.f, 0.f);
		static const RadeonRays::float3 kCameraUp; //= RadeonRays::float3(0.f, 1.f, 0.f);


		// default write buffer size
		static const std::size_t kWriteBufferSize = 4;

		// renderer change scene flag
		std::vector<bool> mChangeSceneFlags;



		// workaround
		RenderingManager* renderManagerPtr = nullptr;

	};



}












#endif









