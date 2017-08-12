#include "SceneTracker.hpp"

#include <iostream>

#include "Scene/Iterator.hpp"
#include "Scene/Shape.hpp"

// test only, should be remove later
#include "Scene/Material.hpp"

namespace SP {


    // helper function for matrix debugging
    static void printMat(const RadeonRays::matrix& mat) {

        // its a 4*4 matrix
        for (int i = 0 ; i < 4; ++i) {

            for (int j = 0 ; j < 4; ++j) {

                std::cerr << "mat[i][j]" << mat.m[i][j] << std::endl;

            }

        }
    }


    // helper function for computing the transformation from projection space to world space
    static void computeProjectionToWorld(RadeonRays::matrix& mat, RadeonRays::matrix& invmat) {




    }



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

				const auto * mesh { static_cast<const Mesh*>(shapeIterator->nextItem())};

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
	void SceneTracker::removeShapesInScene_test() {

		std::cerr << "remove shape start !" << std::endl;

		if (!internalShapes.empty()) {


			RadeonRays::Shape* delShape {internalShapes.back()};
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

		std::cerr << "remove Shapes Commit" << std::endl;

	}

	// test
	void SceneTracker::addShapesInScene_test() {


		std::cerr << "add Shape starts !" << std::endl;


		if (!internalShapes.empty()) {

            // get the reference
            RadeonRays::Shape* refShape {internalShapes.front()};

            // create mesh or instantiate ?
            RadeonRays::Shape* newShape {api->CreateInstance(refShape)};      // Note: blocking call


            // get matrix  (for debug only)
            RadeonRays::matrix matRef;
            RadeonRays::matrix invmatRef;

            refShape->GetTransform(matRef, invmatRef);

            // print out for debugging
            printMat(matRef);


            // create the matrix
            RadeonRays::matrix matNew;
            RadeonRays::matrix invmatNew;

            // compute transformation

            computeProjectionToWorld(matNew, invmatNew);

            // apply transformation to shape
            newShape->SetTransform(matNew, invmatNew);

            // push to vector
            internalShapes.push_back(newShape);

            // add shape
            api->AttachShape(newShape);

            // commit
            api->Commit();

            std::cerr << "Commit shape " << std::endl;


        }

		


		std::cerr << "add shape ends !" << std::endl;


	}

}
