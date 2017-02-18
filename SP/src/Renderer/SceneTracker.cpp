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


}
