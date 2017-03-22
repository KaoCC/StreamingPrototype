#ifndef _SP_CAMERA_HPP_
#define _SP_CAMERA_HPP_

#include "SceneObject.hpp"

#include "math/mathutils.h"

namespace SP {



	class Camera : public SceneObject {
	public:
		Camera() = default;
		virtual ~Camera() = default;
	};


	class PerspectiveCamera : public Camera {
	public:
		// camera position, camera look at, up vector
		PerspectiveCamera(RadeonRays::float3 const& eye, RadeonRays::float3 const& at, RadeonRays::float3 const& up);


		void setSensorSize(RadeonRays::float2 const& size);
		RadeonRays::float2 getSensorSize() const;
		

		void rotate(float angle);

	private:


		// Camera coordinate
		RadeonRays::float3 forwardVec;
		RadeonRays::float3 rightVec;
		RadeonRays::float3 upVec;
		RadeonRays::float3 position;

		// Image plane width & hight in scene units
		RadeonRays::float2 dim;



	};


}




#endif


