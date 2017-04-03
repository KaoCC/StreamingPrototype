#include "Scene.hpp"

#include <vector>
#include <set>

#include "SceneObject.hpp"

#include "Iterator.hpp"

#include "math/mathutils.h"

#include "Light.hpp"

namespace SP {

	// Data structures for shapes and lights
	using ShapeList = std::vector<Shape const*>;
	using LightList = std::vector<Light const*>;
	using AutoreleasePool = std::set<SceneObject const*>;

	struct Scene::SceneImpl {
		ShapeList shapes;
		LightList lights;

		Camera const* camera;

		DirtyFlags dirtyFlags;
		AutoreleasePool autoreleasePool;
	};

	Scene::Scene() : scenePtr(new SceneImpl) {

		scenePtr->camera = nullptr;
		clearDirtyFlags();
	}

	Scene::~Scene() {
		for (auto& element : scenePtr->autoreleasePool) {
			delete element;
		}
	}

	void Scene::attachLight(Light const * light) {

		// find the light !
		LightList::const_iterator citer = std::find(scenePtr->lights.cbegin(), scenePtr->lights.cend(), light);

		// insert if not found
		if (citer == scenePtr->lights.cend()) {
			scenePtr->lights.push_back(light);

			setDirtyFlag(kLights);
		}
	}

	void Scene::setDirtyFlag(DirtyFlags flag) const {
		scenePtr->dirtyFlags = scenePtr->dirtyFlags | flag;
	}

	void Scene::clearDirtyFlags() const {
		scenePtr->dirtyFlags = 0;
	}

	void Scene::attachAutoreleaseObject(SceneObject const * object) {
		// Check if the light is already in the scene
		AutoreleasePool::const_iterator citer = std::find(scenePtr->autoreleasePool.cbegin(), scenePtr->autoreleasePool.cend(), object);

		// And insert only if not
		if (citer == scenePtr->autoreleasePool.cend()) {
			scenePtr->autoreleasePool.insert(object);
		}
	}

	std::size_t Scene::getNumLights() const {
		return scenePtr->lights.size();
	}

	const Light* Scene::getSampleLightPtr(float sample, float & pdf) const {
		size_t numLight = getNumLights();

		if (numLight == 0) {
			return nullptr;
		} else {
			int lightIdx = RadeonRays::clamp((sample * numLight), 0, numLight - 1);
			pdf = 1.f / numLight;
			return scenePtr->lights[lightIdx];
		}
	}

	//RadeonRays::float3 Scene::sampleLight(size_t lightIndex, const DifferentialGeometry & diffGeo, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) const {
	//	const Light* lightInst = scenePtr->lights[lightIndex];


	//	return lightInst->sample(diffGeo, sample, wo, pdf);
	//}

	void Scene::attachShape(Shape const * shape) {
		// Check if the shape is already in the scene
		ShapeList::const_iterator citer = std::find(scenePtr->shapes.cbegin(), scenePtr->shapes.cend(), shape);

		// And attach it if not
		if (citer == scenePtr->shapes.cend()) {
			scenePtr->shapes.push_back(shape);

			setDirtyFlag(kShapes);
		}
	}

	std::size_t Scene::getNumShapes() const {
		return scenePtr->shapes.size();
	}

	Iterator * Scene::createShapeIterator() const {
		return new GenericIterator<ShapeList::const_iterator>(scenePtr->shapes.begin(), scenePtr->shapes.end());
	}

	void Scene::SetCamera(Camera const * cam) {
		scenePtr->camera = cam;
		setDirtyFlag(kCamera);
	}

	Camera const * Scene::getCamera() const {
		return scenePtr->camera;
	}

}
