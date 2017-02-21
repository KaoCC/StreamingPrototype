

#include "SceneIO.hpp"


#include <vector>

// Note: define this in only *one* .cpp
//#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

#include <iostream>

namespace SP {




	class SceneIOImpl : public SceneIO {


	public:
		// Load scene from file
		Scene* loadScene(std::string const& filename, std::string const& basepath) const override;

	private:

	};


	Scene * SceneIOImpl::loadScene(std::string const & filename, std::string const & basepath) const {

		//using namespace tinyobj;

		// Yet To Be Done ! 

		// data buffers
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;

		// load the file

		std::string errorString;
		bool status = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &errorString, filename.c_str(), basepath.c_str());
		if (!errorString.empty()) {
			// may contain warnings
			std::cerr << "Error String: " << errorString << std::endl;
		}

		if (!status) {
			throw std::runtime_error(errorString);
		}

		// more here ! ...

		for (size_t s = 0; s < objShapes.size(); ++s) {


			// yet to be done
		}

		return nullptr;
	}




	SceneIO * SceneIO::createSceneIO() {
		return new SceneIOImpl();
	}
}
