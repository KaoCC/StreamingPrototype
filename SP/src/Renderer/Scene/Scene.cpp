#include "Scene.hpp"

#include <vector>
#include <set>

#include "SceneObject.hpp"

#include "Iterator.hpp"

#include "math/mathutils.h"

#include "Light.hpp"
#include "Camera.hpp"


namespace SP {

	// Data structures for shapes and lights
	using ShapeList = std::vector<std::unique_ptr<Shape const>>;
	using LightList = std::vector<std::unique_ptr<Light const>>;
	//using AutoreleasePool = std::set<SceneObject const*>;

	// support multiple camera
	using CameraList = std::vector<std::unique_ptr<Camera const>>;

	struct Scene::SceneImpl {
		ShapeList shapes;
		LightList lights;

		//Camera const* camera;
		CameraList cameras;

		DirtyFlags dirtyFlags;
		//AutoreleasePool autoreleasePool;
	};

	Scene::Scene() : scenePtr { new SceneImpl } {

		//scenePtr->camera = nullptr;
		clearDirtyFlags();
	}

	Scene::~Scene() = default;

	void Scene::attachLight(std::unique_ptr<Light const> light) {

		// find the light !
		auto citer = std::find(scenePtr->lights.cbegin(), scenePtr->lights.cend(), light);

		// insert if not found
		if (citer == scenePtr->lights.cend()) {
			scenePtr->lights.push_back(std::move(light));

			setDirtyFlag(kLights);
		}
	}

	Scene::DirtyFlags Scene::getDirtyFlags() const {
		return scenePtr->dirtyFlags;
	}

	void Scene::setDirtyFlag(DirtyFlags flag) const {
		scenePtr->dirtyFlags = scenePtr->dirtyFlags | flag;
	}

	void Scene::clearDirtyFlags() const {
		scenePtr->dirtyFlags = 0;
	}

	//void Scene::attachAutoreleaseObject(SceneObject const * object) {
	//	// Check if the light is already in the scene
	//	auto citer = std::find(scenePtr->autoreleasePool.cbegin(), scenePtr->autoreleasePool.cend(), object);

	//	// And insert only if not
	//	if (citer == scenePtr->autoreleasePool.cend()) {
	//		scenePtr->autoreleasePool.insert(object);
	//	}
	//}

	std::size_t Scene::getNumLights() const {
		return scenePtr->lights.size();
	}

	const Light* Scene::getSampleLightPtr(float sample, float & pdf) const {
		std::size_t numLight = getNumLights();

		if (numLight == 0) {
			return nullptr;
		}

		auto lightIdx = static_cast<int>(RadeonRays::clamp((sample * numLight), 0, numLight - 1));
			pdf = 1.f / numLight;
			return scenePtr->lights[lightIdx].get();

	}

	//RadeonRays::float3 Scene::sampleLight(size_t lightIndex, const DifferentialGeometry & diffGeo, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) const {
	//	const Light* lightInst = scenePtr->lights[lightIndex];


	//	return lightInst->sample(diffGeo, sample, wo, pdf);
	//}

	void Scene::attachShape(std::unique_ptr<Shape const> shape) {
		// Check if the shape is already in the scene
		auto citer = std::find(scenePtr->shapes.cbegin(), scenePtr->shapes.cend(), shape);

		// And attach it if not
		if (citer == scenePtr->shapes.cend()) {
			scenePtr->shapes.push_back(std::move(shape));

			setDirtyFlag(kShapes);
		}
	}

	std::size_t Scene::getNumShapes() const {
		return scenePtr->shapes.size();
	}

	std::unique_ptr<Iterator> Scene::createShapeIterator() const {
		return std::make_unique<GenericIterator<ShapeList::const_iterator>>(scenePtr->shapes.begin(), scenePtr->shapes.end());
	}

	const Shape& Scene::getShape(size_t shapeIdx) const {
		return *scenePtr->shapes[shapeIdx];
	}

	void Scene::attachCamera(std::unique_ptr<Camera const> cam) {
		//scenePtr->camera = cam;

		scenePtr->cameras.push_back(std::move(cam));

		setDirtyFlag(kCamera);
	}

	const Camera& Scene::getCamera(size_t camIdx) const {

		if (camIdx < scenePtr->cameras.size()) {

			return *scenePtr->cameras[camIdx];
		}

		throw std::runtime_error("Camera: array index out of bound");


		//return scenePtr->camera;
	}

}
