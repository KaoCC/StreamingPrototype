#include "Scene.hpp"

#include <vector>
#include <set>

#include "SceneObject.hpp"

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

	Scene::Scene() : ScenePtr(new SceneImpl) {

		ScenePtr->camera = nullptr;
		clearDirtyFlags();
	}

	Scene::~Scene() {
		for (auto& element : ScenePtr->autoreleasePool) {
			delete element;
		}
	}

	void Scene::attachLight(Light const * light) {

		// find the light !
		LightList::const_iterator citer = std::find(ScenePtr->lights.cbegin(), ScenePtr->lights.cend(), light);

		// insert if not found
		if (citer == ScenePtr->lights.cend()) {
			ScenePtr->lights.push_back(light);

			setDirtyFlag(kLights);
		}
	}

	void Scene::setDirtyFlag(DirtyFlags flag) const {
		ScenePtr->dirtyFlags = ScenePtr->dirtyFlags | flag;
	}

	void Scene::clearDirtyFlags() const {
		ScenePtr->dirtyFlags = 0;
	}

	void Scene::attachAutoreleaseObject(SceneObject const * object) {
		// Check if the light is already in the scene
		AutoreleasePool::const_iterator citer = std::find(ScenePtr->autoreleasePool.cbegin(), ScenePtr->autoreleasePool.cend(), object);

		// And insert only if not
		if (citer == ScenePtr->autoreleasePool.cend()) {
			ScenePtr->autoreleasePool.insert(object);
		}
	}

	std::size_t Scene::getNumLights() const {
		return ScenePtr->lights.size();
	}

	void Scene::attachShape(Shape const * shape) {
		// Check if the shape is already in the scene
		ShapeList::const_iterator citer = std::find(ScenePtr->shapes.cbegin(), ScenePtr->shapes.cend(), shape);

		// And attach it if not
		if (citer == ScenePtr->shapes.cend()) {
			ScenePtr->shapes.push_back(shape);

			setDirtyFlag(kShapes);
		}
	}

	std::size_t Scene::getNumShapes() const {
		return ScenePtr->shapes.size();
	}

}
