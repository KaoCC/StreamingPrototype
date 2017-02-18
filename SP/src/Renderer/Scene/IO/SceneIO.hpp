#ifndef _SP_SCENEIO_HPP_
#define _SP_SCENEIO_HPP_

#include <string>

#include "../Scene.hpp"

namespace SP {

	class SceneIO {
	public:
		// Create OBJ scene loader
		static SceneIO* createSceneIO();

		// for testing only
		// Create test scene loader
		static SceneIO* createSceneIOTest();

		// Constructor
		SceneIO() = default;
		// Destructor
		virtual ~SceneIO() = default;

		// Load the scene from file using resourse base path
		virtual Scene* loadScene(std::string const& filename, std::string const& basepath) const = 0;

		// Disallow copying
		SceneIO(SceneIO const&) = delete;
		SceneIO& operator = (SceneIO const&) = delete;
	};



}







#endif
