#include "SceneTracker.hpp"


namespace SP {

	SceneTracker::SceneTracker(int devidx) {

		// init API;
		api = RadeonRays::IntersectionApi::Create(devidx);


		// ... 
	}

	SceneTracker::~SceneTracker() {
		RadeonRays::IntersectionApi::Delete(api);
	}


}
