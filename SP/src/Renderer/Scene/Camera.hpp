#ifndef _SP_CAMERA_HPP_
#define _SP_CAMERA_HPP_

#include "SceneObject.hpp"

#include "math/mathutils.h"

namespace SP {



	class Camera : public SceneObject {
	public:
		Camera() = default;
		virtual ~Camera();
	};


	class PerspectiveCamera : public Camera {
	public:
		// camera position, camera look at, up vector
		PerspectiveCamera(RadeonRays::float3 const& eye, RadeonRays::float3 const& at, RadeonRays::float3 const& up);
		virtual ~PerspectiveCamera();

		// Set camera focus distance in meters,
		// this is essentially a distance from the lens to the focal plane.
		void setFocusDistance(float distance);
		float getFocusDistance() const;

		void setAperture(float aperture);
		float getAperture() const;

		void setSensorSize(RadeonRays::float2 const& size);
		RadeonRays::float2 getSensorSize() const;


		// Set camera focal length in meters,
		// this is essentially a distance between a camera sensor and a lens.
		void setFocalLength(float length);
		float getFocalLength() const;

		void setDepthRange(RadeonRays::float2 const& range);
		RadeonRays::float2 getDepthRange() const;

		RadeonRays::float3 getForwardVector() const;
		RadeonRays::float3 getRightVector() const;
		RadeonRays::float3 getUpVector() const;
		RadeonRays::float3 getPosition() const;
		float getAspectRatio() const;


		// trans

		void rotate(float angle);

	private:


		// Camera coordinate
		RadeonRays::float3 forwardVec;
		RadeonRays::float3 rightVec;
		RadeonRays::float3 upVec;
		RadeonRays::float3 position;

		// Image plane width & hight in scene units
		RadeonRays::float2 dim;


		// Near and far Z
		RadeonRays::float2 zcap;

		float focalLength = 0.f;
		float aspect;
		float focusDistance = 0.f;
		float aperture = 0.f;

	};


}




#endif


