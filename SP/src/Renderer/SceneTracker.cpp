#include "SceneTracker.hpp"

#include <iostream>

#include "Scene/Iterator.hpp"
#include "Scene/Shape.hpp"

// test only, should be remove later
#include "Scene/Material.hpp"

namespace SP {

	SceneTracker::SceneTracker(RadeonRays::IntersectionApi* intersectApi) : api{ intersectApi } {


	}

	//SceneTracker::~SceneTracker() {
	//	RadeonRays::IntersectionApi::Delete(api);
	//}

	//RadeonRays::IntersectionApi * SceneTracker::getIntersectionApi() {
	//	return api;
	//}

	void SceneTracker::compileSceneTest(const Scene& scene) {
		// yet to be done


		// this is for testing only

		// first time ?
		if (currentScenePtr == nullptr) {
			currentScenePtr = &scene;


			internalMeshPtrs.clear();

			std::unique_ptr<Iterator> shapeIterator(scene.createShapeIterator());

			// create mesh

			//std::cerr << "While" << std::endl;

			int shapeID = 1;
			while (shapeIterator->hasNext()) {

				// get mesh ?

				const auto * mesh = static_cast<const Mesh*>(shapeIterator->nextItem());

				RadeonRays::Shape* shape = api->CreateMesh(
					reinterpret_cast<const float*>(mesh->getVertices()),			// check this one !!!
					static_cast<int>(mesh->getNumVertices()),
					sizeof(RadeonRays::float3),
					reinterpret_cast<const int*>(mesh->getIndices()),
					0,
					nullptr,
					static_cast<int>(mesh->getNumIndices() / 3)
				);



				shape->SetId(shapeID);
				++shapeID;

				internalShapes.push_back(shape);

				//test
				internalMeshPtrs.push_back(mesh);
			}

			std::cerr << "Num of internal Shapes: " << internalShapes.size() << std::endl;


			// TEST !!!!!
			for (const auto& s : internalShapes) {
				api->AttachShape(s);
				std::cerr << "Add shape !" << std::endl;
			}

			api->Commit();
			std::cerr << "Commit" << std::endl;

		}


		// test
		if (currentScenePtr != &scene) {
			// detatch all
			std::cerr << "The scene has been changed !" << std::endl;
		}

		// can cause error !
		// cannot commit twice ?
		//api->Commit();

	}

	const std::vector<const Mesh*>& SceneTracker::getInternalMeshPtrs() const {
		return internalMeshPtrs;
	}

	const Scene * SceneTracker::getCurrentScenePtr() const {
		return currentScenePtr;
	}

	// for testing only
	void SceneTracker::changeShapesInScene_test() {

		std::cerr << "change shape start !" << std::endl;

		if (!internalShapes.empty()) {


			auto delShape = internalShapes.back();
			internalShapes.pop_back();

			size_t sz = internalShapes.size();

			std::cerr << "Number of shapes left:" << sz << std::endl;

			if (sz > 0) {

				// tests
				if (!internalMeshPtrs[sz-1]->getMaterial()->hasEmission()) {
					api->DetachShape(delShape);
					api->DeleteShape(delShape);

					api->Commit();

				} else {
					std::cerr << "<<<<<< This is the Light ! ..." << std::endl;
				}

			}

		} else {
			std::cerr << " >>>>>>>>>>> Empty Scene ......" << std::endl;
		}

		std::cerr << "change Shapes Commit" << std::endl;

	}

}
