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

	AreaLight::AreaLight(Shape const * sh, size_t idx) : shape(sh), primitiveIndex (idx) {
	}

	const Shape * AreaLight::getShape() const {
		return shape;
	}

	size_t AreaLight::getPrimitiveIndex() const {
		return primitiveIndex;
	}

	RadeonRays::float3 AreaLight::sample(const DifferentialGeometry & diffGeo, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) const {

		size_t primId = getPrimitiveIndex();
		const Mesh* meshDataPtr = dynamic_cast<const Mesh*>(getShape());

		const uint32_t* indexArray = meshDataPtr->getIndices();

		uint32_t i0 = indexArray[3 * primId];								// CHECK ?
		uint32_t i1 = indexArray[3 * primId + 1];
		uint32_t i2 = indexArray[3 * primId + 2];

		const RadeonRays::float3* normalArray = meshDataPtr->getNormals();

		RadeonRays::float3 n0 = normalArray[i0];
		RadeonRays::float3 n1 = normalArray[i1];
		RadeonRays::float3 n2 = normalArray[i2];


		const RadeonRays::float3* verticeArray = meshDataPtr->getVertices();

		RadeonRays::float3 v0 = verticeArray[i0];
		RadeonRays::float3 v1 = verticeArray[i1];
		RadeonRays::float3 v2 = verticeArray[i2];


		const RadeonRays::float2* uvArray = meshDataPtr->getUVs();

		RadeonRays::float2 uv0 = uvArray[i0];
		RadeonRays::float2 uv1 = uvArray[i1];
		RadeonRays::float2 uv2 = uvArray[i2];


		// Generate sample on triangle
		float r0 = sample.x;
		float r1 = sample.y;

		RadeonRays::float2 localUV;
		localUV.x = std::sqrt(r0) * (1.f - r1);
		localUV.y = std::sqrt(r0) * r1;

		// Calculate barycentric position and normal
		RadeonRays::float3 n = RadeonRays::normalize((1.f - localUV.x - localUV.y) * n0 + localUV.x * n1 + localUV.y * n2);
		RadeonRays::float3 p = (1.f - localUV.x - localUV.y) * v0 + localUV.x * v1 + localUV.y * v2;
		RadeonRays::float2 tx = (1.f - localUV.x - localUV.y) * uv0 + localUV.x * uv1 + localUV.y * uv2;

		wo = p - diffGeo.getPosition();
		pdf = 1.f / std::sqrt(((RadeonRays::cross(v2 - v0, v2 - v1)) * 0.5f).sqnorm());

		const Material* matPtr = getShape()->getMaterial();


		// Texture_GetValue3f(mat.kx.xyz, tx, TEXTURE_ARGS_IDX(mat.kxmapidx));
		// KAOCC: texture is not supported yet ...
		const RadeonRays::float3 ke = matPtr->getInputValue("albedo").floatValue;

		RadeonRays::float3 v = -RadeonRays::normalize(wo);

		float n_dot_v = dot(n, v);

		if (n_dot_v > 0.f) {
			float denom = (std::sqrt(wo.sqnorm()) * std::sqrt(wo.sqnorm()));
			return denom > 0.f ? (ke * n_dot_v * (1 / denom)) : 0.f;
		} else {
			pdf = 0.f;
			return 0.f;
		}


	}

	bool AreaLight::isSingular() const {
		return false;
	}

}
