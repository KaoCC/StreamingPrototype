

#include "SceneIO.hpp"




namespace SP {


	SceneIO * SceneIO::createSceneIO() {
		return new SceneIOImpl();
	}


	class SceneIOImpl : public SceneIO {


	public:
		// Load scene from file
		Scene* loadScene(std::string const& filename, std::string const& basepath) const override;

	private:

	};



	Scene * SceneIOImpl::loadScene(std::string const & filename, std::string const & basepath) const {


		// Yet To Be Done ! 



		return nullptr;
	}

}
