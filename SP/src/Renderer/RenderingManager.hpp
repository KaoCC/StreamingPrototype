#ifndef _SP_RENDERINGMANAGER_HPP_
#define _SP_RENDERINGMANAGER_HPP_

#include "Scene/Camera.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <memory>
#include <string>

#include "Scene/Scene.hpp"
#include "../SyncBuffer.hpp"
#include "Common.hpp"
#include "PtRenderer.hpp"

#include "../Encoder/Encoder.hpp"

namespace SP {



	class RenderingManager {

	public:

		RenderingManager() = delete;


		RenderingManager(SyncBuffer<ImageConfig>& buff);
		~RenderingManager();

		void startRenderThread();

	private:


		// for testing

		void testOutput(int id);

		// helper function
		void initData();


		// Camera
		//std::unique_ptr<SP::Camera> camera;

		// parameters
		const int windowWidth = 512;
		const int windowHeight = 512;

		const int numberOfBounce = 5;


		// tmp
		const std::string defaultPath = "../Resources/CornellBox";
		const std::string defaultModelname = "orig.objm";


		// Scene Data
		std::unique_ptr<Scene> sceneDataPtr;


		// output buffer ref
		SyncBuffer<ImageConfig>& syncBuffer;

		// thread
		std::unique_ptr<std::thread> renderThread;

		// Path-Tracing Renderer
		std::unique_ptr<PtRenderer> renderer;

		//Encoder* encoder;
		//ImageConfig::ImageBuffer accImageBuffer; // test


	};


}





#endif



