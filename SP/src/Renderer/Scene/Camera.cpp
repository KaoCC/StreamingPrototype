#include "Camera.hpp"

namespace SP {

	PerspectiveCamera::PerspectiveCamera(RadeonRays::float3 const & eye, RadeonRays::float3 const & at, RadeonRays::float3 const & up)
		: position { eye } {

		forwardVec = normalize(at - eye);
		rightVec = cross(forwardVec, normalize(up));
		upVec = cross(rightVec, forwardVec);
	}

	void PerspectiveCamera::setFocusDistance(float distance) {
		focusDistance = distance;
		setDirty(true);
	}

	float PerspectiveCamera::getFocusDistance() const {
		return focusDistance;
	}

	void PerspectiveCamera::setAperture(float aper) {
		aperture = aper;
		setDirty(true);
	}

	float PerspectiveCamera::getAperture() const {
		return aperture;
	}

	void PerspectiveCamera::setSensorSize(RadeonRays::float2 const & size) {
		dim = size;
		setDirty(true);
	}

	RadeonRays::float2 PerspectiveCamera::getSensorSize() const {
		return dim;
	}

	void PerspectiveCamera::setFocalLength(float length) {
		focalLength = length;
		setDirty(true);
	}

	float PerspectiveCamera::getFocalLength() const {
		return focalLength;
	}

	void PerspectiveCamera::setDepthRange(RadeonRays::float2 const & range) {
		zcap = range;
		setDirty(true);
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

	float PerspectiveCamera::getAspectRatio() const {
		return aspect;
	}

}
