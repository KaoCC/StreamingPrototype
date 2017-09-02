#include "SceneTracker.hpp"

#include <iostream>

#include "Scene/Iterator.hpp"
#include "Scene/Shape.hpp"

// test only, should be remove later
#include "Scene/Material.hpp"

namespace SP {

	// ------ test ------

	// helper function for matrix debugging 
//	static void printMat(const RadeonRays::matrix& mat) {
//
//		// its a 4*4 matrix 
//		for (int i = 0; i < 4; ++i) {
//
//			for (int j = 0; j < 4; ++j) {
//
////std::cerr << "mat[i][j]" << mat.m[i][j] << std::endl;
//
//std::printf("mat[%d][%d]: %f ", i, j, mat.m[i][j]);
//
//			}
//
//			std::cerr << std::endl;
//
//		}
//	}



	// ------ end test ------


	SceneTracker::SceneTracker(const std::vector<RadeonRays::IntersectionApi*>& apis) {

		for (const auto& api : apis) {
			Attribute attr;
			attr.api = api;
			mAttributes.push_back(std::move(attr));
		}

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


			//internalMeshPtrs.clear();

			//std::unique_ptr<Iterator> shapeIterator { scene.createShapeIterator() };

			createShapeRR(scene);

		} else {

			// update shape
			if (scene.getDirtyFlags() & Scene::kShapes) {


				//updateShapes(scene);
				recompileShapes(scene);

			}

		}

		//KAOCC: clear dirty flag ?


		// test
		if (currentScenePtr != &scene) {
			// detatch all
			std::cerr << "The scene has been changed !" << std::endl;
		}


	}

	void SceneTracker::updateShapes(const Scene & scene) {

		throw std::runtime_error("updateShapes: yet to be done !");
	}

	void SceneTracker::recompileShapes(const Scene & scene) {

		std::cerr << ">>>>>>>>>>>>>>>>>>>> Recompile\n";

		clearAll();
		createShapeRR(scene);
	}

	// test
	void SceneTracker::clearAll() {
		for (auto& attr : mAttributes) {

			for (auto& shape : attr.shapes) {
				attr.api->DetachShape(shape);
				attr.api->DeleteShape(shape);
			}

			// check for memory leak
			attr.shapes.clear();
		}
	}

	void SceneTracker::createShapeRR(const Scene& scene) {


		//std::vector<RadeonRays::Shape*> internalShapes;		// check for memory leaks

		std::unique_ptr<Iterator> shapeIterator { scene.createShapeIterator() };

		int shapeID = 1;
		while (shapeIterator->hasNext()) {

			// get mesh ?
			const auto* mesh = static_cast<const Mesh*>(shapeIterator->nextItem());

			for (auto& attribute : mAttributes) {

				RadeonRays::Shape* shape = attribute.api->CreateMesh(
					reinterpret_cast<const float*>(mesh->getVertices()),            // check this one !!!
					static_cast<int>(mesh->getNumVertices()),
					sizeof(RadeonRays::float3),
					reinterpret_cast<const int*>(mesh->getIndices()),
					0,
					nullptr,
					static_cast<int>(mesh->getNumIndices() / 3)
				);


				shape->SetId(shapeID);

				//internalShapes.push_back(shape);
				attribute.api->AttachShape(shape);
				std::cerr << "Add shape !" << std::endl;

				// add to map
				//attribute.lookupTable[mesh] = shape;

				// add to list
				attribute.shapes.push_back(shape);

			}

			++shapeID;

		}

		std::cerr << "Num of internal Shapes: " << shapeID - 1 << std::endl;


		// TEST !!!!!
		//for (const auto& s : internalShapes) {
		//	api->AttachShape(s);
		//	std::cerr << "Add shape !" << std::endl;
		//}
		
		for (auto& attribute: mAttributes) {
			attribute.api->Commit();
			std::cerr << "Commit" << std::endl;
		}
	}



	//const std::vector<const Mesh*>& SceneTracker::getInternalMeshPtrs() const {
	//	return internalMeshPtrs;
	//}

	//const Scene* SceneTracker::getCurrentScenePtr() const {
	//	return currentScenePtr;
	//}

	// for testing only
	//void SceneTracker::removeShapesInScene_test() {

	//	std::cerr << "remove shape start !" << std::endl;

	//	if (!internalShapes.empty()) {


	//		RadeonRays::Shape* delShape { internalShapes.back() };
	//		internalShapes.pop_back();

	//		size_t sz = internalShapes.size();

	//		std::cerr << "Number of shapes left:" << sz << std::endl;

	//		if (sz > 0) {

	//			// tests
	//			if (!internalMeshPtrs[sz - 1]->getMaterial()->hasEmission()) {
	//				api->DetachShape(delShape);
	//				api->DeleteShape(delShape);

	//				api->Commit();

	//			} else {
	//				std::cerr << "<<<<<< This is the Light ! ..." << std::endl;
	//			}

	//		}

	//	} else {
	//		std::cerr << " >>>>>>>>>>> Empty Scene ......" << std::endl;
	//	}

	//	std::cerr << "remove Shapes Commit" << std::endl;

	//}

	// test !!!


	// test
	//void SceneTracker::addShapesInScene_test(float worldX, float worldY, float worldZ) {

	//	// test !
	//	//static bool initFlag = true;

	//	std::cerr << "add Shape starts !" << std::endl;


	//	if (!internalShapes.empty()) {

	//		// get the reference
	//		//RadeonRays::Shape& refShape { *internalShapes.front() };
	//		//const SP::Mesh& refMesh { *internalMeshPtrs.front() };
	//		const SP::Material& refMat{ *(internalMeshPtrs[44]->getMaterial()) };

	//		// add default mesh !!
	//		SP::Mesh* defaultMesh {createDefaultMesh(worldX, worldY, worldZ)};

	//		// workaround
	//		defaultMesh->setMaterial(&refMat);

	//		RadeonRays::Shape* defaultShape = api->CreateMesh(
	//			reinterpret_cast<const float*>(defaultMesh->getVertices()),            // check this one !!!
	//			static_cast<int>(defaultMesh->getNumVertices()),
	//			sizeof(RadeonRays::float3),
	//			reinterpret_cast<const int*>(defaultMesh->getIndices()),
	//			0,
	//			nullptr,
	//			static_cast<int>(defaultMesh->getNumIndices() / 3)
	//		);

	//		// create mesh or instantiate ?
	//		//RadeonRays::Shape* newShape { api->CreateInstance(&refShape) };      // Note: blocking call

	//		// get matrix  (for debug only)
	//		//RadeonRays::matrix matRef;
	//		//RadeonRays::matrix invmatRef;

	//		//refShape.GetTransform(matRef, invmatRef);

	//		// print out for debugging
	//		//std::cerr << "Mat Ref: \n";
	//		//printMat(matRef);


	//		// compute world space position from ST coordinates (world space coordinate = inv Projection Matrix (with depth info) * ST coordinate )
	//		//RadeonRays::float3 worldPosition = computeProjectionToWorld();


	//		// compute translation matrix

	//		std::printf("worldx, worldy, worldz: %f, %f, %f\n", worldX, worldY, worldZ);

	//		//RadeonRays::matrix matNew {
	//		//	1, 0, 0, worldX,
	//		//	0, 1, 0, worldY,
	//		//	0, 0, 1, worldZ,
	//		//	0, 0, 0, 1
	//		//};

	//		//RadeonRays::matrix invmatNew = RadeonRays::inverse(matNew);

	//		//computeTransformation(computeProjectionToWorld(x, y), matNew, invmatNew);


	//		//std::cerr << "Mat New\n";
	//		// print out for debugging
	//		//printMat(matNew);

	//		// apply transformation to shape
	//		//newShape->SetTransform(matNew, invmatNew);


	//		//if (!initFlag) {

	//		//	// remove shape
	//		//	auto currentShape = internalShapes.back();
	//		//	internalShapes.pop_back();
	//		//	api->DetachShape(currentShape);
	//		//	api->DeleteShape(currentShape);

	//		//	internalMeshPtrs.pop_back();

	//		//} else {
	//		//	initFlag = false;
	//		//}

	//		// push to vector
	//		internalShapes.push_back(defaultShape);
	//		//internalMeshPtrs.push_back(defaultMesh);

	//		// be careful !
	//		defaultShape->SetId(internalShapes.size());

	//		// add shape
	//		api->AttachShape(defaultShape);

	//		// commit
	//		api->Commit();

	//		std::cerr << "Commit shape " << std::endl;

	//	}


	//	std::cerr << "add shape ends !" << std::endl;

	//}

	//void SceneTracker::addShapeTransform_test(float worldX, float worldY, float worldZ) {





	//}





}
