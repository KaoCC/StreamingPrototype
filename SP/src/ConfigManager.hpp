#ifndef _SP_CONFIGMANAGER_HPP_
#define _SP_CONFIGMANAGER_HPP_

#include "Common.hpp"

#include <vector>

namespace SP {

	class ConfigManager {


	public:
		ConfigManager();

		CameraConfig getCamera();

		void setScreen(uint32_t w, uint32_t h);
		ScreenConfig getScreen();

		void setModuleID(uint32_t mID);

		void setPositionDelta(float dx, float dy, float dz);

		const ImageConfig& getImageRef(size_t index);


		// tmp
		void loadImages();

	private:

		uint32_t moduleID = 0;
		
		CameraConfig cameraCfg;
		ScreenConfig screenCfg;

		std::vector<ImageConfig> images;

	};



}












#endif









