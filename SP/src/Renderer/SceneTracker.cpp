#include "SceneTracker.hpp"

#include <iostream>

#include "Scene/Iterator.hpp"
#include "Scene/Shape.hpp"

namespace SP {

	SceneTracker::SceneTracker(int devidx) {

		std::cerr << "API" << std::endl;

		// init API;
		api = RadeonRays::IntersectionApi::Create(devidx);

		std::cerr << "Done" << std::endl;

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


			internalMeshPtrs.clear();

			std::unique_ptr<Iterator> shapeIterator(scene.createShapeIterator());

			// create mesh

			//std::cerr << "While" << std::endl;

			int shapeID = 1;
			while (shapeIterator->hasNext()) {

				// get mesh ?

				const Mesh* mesh = static_cast<const Mesh*>(shapeIterator->nextItem());

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

	const std::vector<const Mesh*>& SceneTracker::getInternalMeshPtrs() const{
		return internalMeshPtrs;
	}

	const Scene * SceneTracker::getCurrentScenePtr() const {
		return currentScenePtr;
	}

}
