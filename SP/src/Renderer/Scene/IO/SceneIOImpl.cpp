

#include "SceneIO.hpp"


#include <vector>

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
		//std::vector<shape_t> objshapes;
		//std::vector<material_t> objmaterials;

		return nullptr;
	}




	SceneIO * SceneIO::createSceneIO() {
		return new SceneIOImpl();
	}
}
