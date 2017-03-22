#include "Camera.hpp"

namespace SP {

	PerspectiveCamera::PerspectiveCamera(RadeonRays::float3 const & eye, RadeonRays::float3 const & at, RadeonRays::float3 const & up) 
		: position(eye) {

		forwardVec = normalize(at - eye);
		rightVec = cross(forwardVec, normalize(up));
		upVec = cross(rightVec, forwardVec);
	}

	RadeonRays::float2 PerspectiveCamera::getSensorSize() const {
		return dim;
	}

}
