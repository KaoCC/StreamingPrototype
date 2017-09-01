#ifndef _SP_SECENETRACKER_HPP_
#define _SP_SECENETRACKER_HPP_

#include "radeon_rays.h"

#include "Scene/Scene.hpp"

#include "Scene/Shape.hpp"

#include <map>

namespace SP {

	class SceneTracker {

	public:

		// should be extendable.. (T & I , RR ...)

		SceneTracker(const std::vector<RadeonRays::IntersectionApi*>& apis);

		SceneTracker(const SceneTracker&) = delete;

		//allow move
		SceneTracker(SceneTracker&&) = default;

		//virtual ~SceneTracker();

		//RadeonRays::IntersectionApi* getIntersectionApi();


		void compileSceneTest(const Scene& scene);


		// test
		//const std::vector<const Mesh*>& getInternalMeshPtrs() const;

		// tmp
		//const Scene* getCurrentScenePtr() const;

		// test
		//void removeShapesInScene_test();

		// test
		//void addShapesInScene_test(float worldX, float worldY, float worldZ);


		//void addShapeTransform_test(float worldX, float worldY, float worldZ);


	private:


		struct Attribute {
			RadeonRays::IntersectionApi* api;
			std::map<const Mesh*, RadeonRays::Shape*> lookupTable;
		};

		//void updateMaterials();

		void updateShapes(const Scene& scene);


		// helper function
		void createShapeRR(const Scene& scene);

		// index of the device ?

		// Intersection API for RR
		std::vector<Attribute> mAttributes;

		// track curent scene
		const Scene* currentScenePtr = nullptr;

		// For RR shape collections
		//std::vector<RadeonRays::Shape*> internalShapes;


		// tmp
		// For Back Tracking Mesh (Shape) Data
		//std::vector<const Mesh*> internalMeshPtrs;




	};


}


#endif

