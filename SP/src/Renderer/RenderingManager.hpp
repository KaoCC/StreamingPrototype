#ifndef _SP_RENDERINGMANAGER_HPP_
#define _SP_RENDERINGMANAGER_HPP_

#include "Scene/Camera.hpp"

#include <memory>
#include <thread>
#include <vector>

namespace SP {



	class RenderingManager {


		RenderingManager();

	private:


		// helper function
		void initData();


		// Camera

		// Rendering Thread


		// parameters
		const int windowWidth = 512;
		const int windowHeight = 512;

		const int numberOfBounce = 5;

	};


}





#endif



