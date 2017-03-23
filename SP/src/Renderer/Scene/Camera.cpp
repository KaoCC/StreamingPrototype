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

	float PerspectiveCamera::getFocalLength() const {
		return focalLength;
	}

	RadeonRays::float2 PerspectiveCamera::getDepthRange() const {
		return zcap;
	}

	RadeonRays::float3 PerspectiveCamera::getForwardVector() const {
		return forwardVec;
	}

	RadeonRays::float3 PerspectiveCamera::getRightVector() const {
		return rightVec;
	}

	RadeonRays::float3 PerspectiveCamera::getUpVector() const {
		return upVec;
	}

	RadeonRays::float3 PerspectiveCamera::getPosition() const {
		return position;
	}

}
