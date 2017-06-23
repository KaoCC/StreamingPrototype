#ifndef _SP_SECENETRACKER_HPP_
#define _SP_SECENETRACKER_HPP_

#include "radeon_rays.h"

#include "Scene/Scene.hpp"

#include "Scene/Shape.hpp"

namespace SP {

	class SceneTracker {

	public:

		SceneTracker(RadeonRays::IntersectionApi* intersectApi);

		//virtual ~SceneTracker();

		//RadeonRays::IntersectionApi* getIntersectionApi();


		void compileSceneTest(const Scene& scene);


		// test
		const std::vector<const Mesh*>& getInternalMeshPtrs() const;
		// tmp
		const Scene* getCurrentScenePtr() const;

	private:

		void updateMaterials();


		// index of the device ?

		// Intersection API for RR
		RadeonRays::IntersectionApi*& apiRef;

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

