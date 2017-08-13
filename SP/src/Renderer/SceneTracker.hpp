#ifndef _SP_SECENETRACKER_HPP_
#define _SP_SECENETRACKER_HPP_

#include "radeon_rays.h"

#include "Scene/Scene.hpp"

#include "Scene/Shape.hpp"

namespace SP {

	class SceneTracker {

	public:

		SceneTracker(RadeonRays::IntersectionApi* intersectApi);

		SceneTracker(const SceneTracker&) = delete;

		//allow move
		SceneTracker(SceneTracker&&) = default;

		//virtual ~SceneTracker();

		//RadeonRays::IntersectionApi* getIntersectionApi();


		void compileSceneTest(const Scene& scene);


		// test
		const std::vector<const Mesh*>& getInternalMeshPtrs() const;

		// tmp
		const Scene* getCurrentScenePtr() const;

		// test
		void removeShapesInScene_test();

		// test
		void addShapesInScene_test(float x, float y);


	private:

		void updateMaterials();


		// index of the device ?

		// Intersection API for RR
		RadeonRays::IntersectionApi* api;

		// track curent scene
		const Scene* currentScenePtr = nullptr;

		// For RR shape collections
		std::vector<RadeonRays::Shape*> internalShapes;


		// tmp
		// For Back Tracking Mesh (Shape) Data
		std::vector<const Mesh*> internalMeshPtrs;


	};


}


#endif

