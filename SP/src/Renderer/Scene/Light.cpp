#include "Light.hpp"


namespace SP {

	// default

	static const RadeonRays::float3 kDefaultPos{ 0.f, 0.f, 0.f };
	static const RadeonRays::float3 kDefaultDir{ 0.f, -1.f, 0.f };
	static const RadeonRays::float3 kDefaultRad{ 1.f, 1.f, 1.f };

	Light::Light() : pos(kDefaultPos), dir(kDefaultDir), radiance(kDefaultRad) {
	}

	RadeonRays::float3 Light::getPosition() const {
		return pos;
	}

	void Light::setPosition(const RadeonRays::float3 & p) {
		pos = p;
		setDirty(true);
	}

	RadeonRays::float3 Light::getDirection() const {
		return dir;
	}

	void Light::setDirection(const RadeonRays::float3 & d) {
		dir = normalize(d);
		setDirty(true);
	}

	RadeonRays::float3 Light::getRadiance() const {
		return radiance;
	}

	void Light::setRadiance(const RadeonRays::float3 & e) {
		radiance = e;
		setDirty(true);
	}

	AreaLight::AreaLight(Shape const * sh, size_t idx) : shape(sh), primitiveIndex (idx){
	}

	const Shape * AreaLight::getShape() const {
		return shape;
	}

	size_t AreaLight::getPrimitiveIndex() const {
		return primitiveIndex;
	}

}
