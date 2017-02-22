#include "SceneTracker.hpp"


namespace SP {

	SceneTracker::SceneTracker(int devidx) {

		// init API;
		api = RadeonRays::IntersectionApi::Create(devidx);


		// ... 

		api->SetOption("acc.type", "fatbvh");
		api->SetOption("bvh.builder", "sah");
	}

	SceneTracker::~SceneTracker() {
		RadeonRays::IntersectionApi::Delete(api);
	}

	RadeonRays::IntersectionApi * SceneTracker::getIntersectionApi() {
		return api;
	}

	void SceneTracker::compileSceneTest(const Scene& scene) {
		// yet to be done


		// this is for testing only

		// first time ?
		if (currentScenePtr == nullptr) {
			currentScenePtr = &scene;


			// create mesh

		}


		// test
		if (currentScenePtr != &scene) {
			// detatch all
		}


		api->Commit();

	}


}
