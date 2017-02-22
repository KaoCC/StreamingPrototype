#ifndef _SP_RENDERINGMANAGER_HPP_
#define _SP_RENDERINGMANAGER_HPP_

#include "Scene/Camera.hpp"

#include <memory>
#include <thread>
#include <vector>

#include <string>

#include "Scene/Scene.hpp"

namespace SP {



	class RenderingManager {

	public:

		RenderingManager();

	private:


		// helper function
		void initData();


		// Camera
		//std::unique_ptr<SP::Camera> camera;

		// Rendering Thread


		// parameters
		const int windowWidth = 512;
		const int windowHeight = 512;

		const int numberOfBounce = 5;


		// tmp
		const std::string defaultPath = "../Resources/CornellBox";
		const std::string defaultModelname = "orig.objm";

		std::unique_ptr<SP::Scene> sceneDataPtr;

	};


}





#endif



