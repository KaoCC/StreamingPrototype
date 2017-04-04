#ifndef _SP_SCENE_HPP_
#define _SP_SCENE_HPP_

#include <memory>

//#include "../DifferentialGeometry.hpp"

#include "math/float3.h"
#include "math/float2.h"

namespace SP {

	class Light;
	class Shape;
	class Volume;
	class Camera;
	class Iterator;
	class SceneObject;

	class DifferentialGeometry;

	class Scene {

	public:

		// Dirty flags are used to perform partial buffer updates to save traffic
		using DirtyFlags = std::uint32_t;
		enum {
			kNone,
			kLights,
			kShapes,
			kShapeTransforms,
			kCamera
		};

		// Constructor
		Scene();
		// Destructor
		~Scene();

		// Add or remove lights
		void attachLight(Light const* light);
		void detachLight(Light const* light);

		// Get the number of lights in the scene
		std::size_t getNumLights() const;
		// Get light iterator
		Iterator* createLightIterator() const;

		// Helper for light sampling
		const Light* getSampleLightPtr(float sample, float& pdf) const;

		// no need anymore !
		//RadeonRays::float3 sampleLight(size_t lightIndex, const DifferentialGeometry& diffGeo, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf) const;
		

		// Add or remove shapes
		void attachShape(Shape const* shape);
		void detachShape(Shape const* shape);

		// Get number of shapes in the scene
		std::size_t getNumShapes() const;
		// Get shape iterator
		Iterator* createShapeIterator() const;

		// Set and get camera
		void attachCamera(Camera const* camera);
		Camera const* getCamera(size_t camIdx) const;

		// Get state change since last clear
		DirtyFlags getDirtyFlags() const;
		// Set specified flag in dirty state
		void setDirtyFlag(DirtyFlags flag) const;
		// Clear all flags
		void clearDirtyFlags() const;

		// Check if the scene is ready for rendering
		bool isValid() const;

		// Autorelase objects are deleted when scene is destroyed
		void attachAutoreleaseObject(SceneObject const* object);
		void detachAutoreleaseObject(SceneObject const* object);

		// Forbidden stuff
		Scene(Scene const&) = delete;
		Scene& operator = (Scene const&) = delete;


	private:
		struct SceneImpl;
		std::unique_ptr<SceneImpl> scenePtr;
	};


}









#endif

