#ifndef _SP_SECENETRACKER_HPP_
#define _SP_SECENETRACKER_HPP_

#include "radeon_rays.h"

#include "Scene/Scene.hpp"

namespace SP {

	class SceneTracker {

	public:

		SceneTracker(int devidx);

		virtual ~SceneTracker();

		RadeonRays::IntersectionApi* getIntersectionApi();


		void compileSceneTest(const Scene& scene);

	private:

		// Intersection API for RR
		RadeonRays::IntersectionApi* api;

		// track curent scene
		const Scene* currentScenePtr = nullptr;

		// For RR shape collections
		std::vector<RadeonRays::Shape*> internalShapes;

	};







}















#endif

