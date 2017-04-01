#include "DifferentialGeometry.hpp"

#include "Scene/Shape.hpp"

namespace SP {

	void DifferentialGeometry::fill(const RadeonRays::Intersection & isectRef, const std::vector<const Mesh*>& meshPtrs) {

		int shapeId = isectRef.shapeid - 1;			//CHECK !
		int primId = isectRef.primid;								// CHECK !!!

		RadeonRays::float2 uv;
		uv.x = isectRef.uvwt.x;
		uv.y = isectRef.uvwt.y;


		// shape (Mesh)
		const Mesh* meshDataPtr = meshPtrs[shapeId];

		const uint32_t* indexArray = meshDataPtr->getIndices();

		// This is the core of this function !
		// if this goes wrong then ... boom !
		uint32_t i0 = indexArray[3 * primId];								// CHECK ?
		uint32_t i1 = indexArray[3 * primId + 1];
		uint32_t i2 = indexArray[3 * primId + 2];

		const RadeonRays::float3* normalArray = meshDataPtr->getNormals();

		RadeonRays::float3 n0 = normalArray[i0];							// This is WRONG !!!
		RadeonRays::float3 n1 = normalArray[i1];							// This is WRONG !!!
		RadeonRays::float3 n2 = normalArray[i2];							// This is WRONG !!!


		const RadeonRays::float3* verticeArray = meshDataPtr->getVertices();

		RadeonRays::float3 v0 = verticeArray[i0];							// This is WRONG !!!
		RadeonRays::float3 v1 = verticeArray[i1];							// This is WRONG !!!
		RadeonRays::float3 v2 = verticeArray[i2];							// This is WRONG !!!


		const RadeonRays::float2* uvArray = meshDataPtr->getUVs();

		RadeonRays::float2 uv0 = uvArray[i0];
		RadeonRays::float2 uv1 = uvArray[i1];
		RadeonRays::float2 uv2 = uvArray[i2];

		RadeonRays::matrix matrixI;			// I

		// normal, position, uv, ng
		normal = RadeonRays::normalize(RadeonRays::transform_vector((1.f - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2, matrixI));  //CHECK THIS !
		pos = RadeonRays::transform_point((1.f - uv.x - uv.y) * v0 + uv.x * v1 + uv.y * v2, matrixI);
		uv = (1.f - uv.x - uv.y) * uv0 + uv.x * uv1 + uv.y * uv2;
		normalGeo = RadeonRays::normalize(RadeonRays::cross(v1 - v0, v2 - v0));


		if (RadeonRays::dot(normalGeo, normal) < 0) {
			normalGeo = -normalGeo;
		}

		// Material ?
		matPtr = meshDataPtr->getMaterial();


		float du1 = uv0.x - uv2.x;
		float du2 = uv1.x - uv2.x;
		float dv1 = uv0.y - uv2.y;
		float dv2 = uv1.y - uv2.y;

		RadeonRays::float3 dp1 = v0 - v2;
		RadeonRays::float3 dp2 = v1 - v2;

		//float determin = du1 * dv2 - dv1 * du2;

		// dpdu ???
		// missing !!

		// dpdv
		dpdv = RadeonRays::normalize(RadeonRays::cross(normal, dpdu));

		// For area calculation
		RadeonRays::float3 p0 = RadeonRays::transform_point(v0, matrixI);
		RadeonRays::float3 p1 = RadeonRays::transform_point(v1, matrixI);
		RadeonRays::float3 p2 = RadeonRays::transform_point(v2, matrixI);


		area = 0.5 * std::sqrt(RadeonRays::cross(p2 - p0, p2 - p1).sqnorm());   // check the value !

	}

	RadeonRays::float3& DifferentialGeometry::getPosition() {
		return pos;
	}

	RadeonRays::float3& DifferentialGeometry::getNormal() {
		return normal;
	}

	RadeonRays::float3& DifferentialGeometry::getNormalGeo() {
		return normalGeo;
	}

	RadeonRays::float3& DifferentialGeometry::getUV() {
		return uv;
	}

	RadeonRays::float3 & DifferentialGeometry::getDpDu() {
		return dpdu;
	}

	RadeonRays::float3 & DifferentialGeometry::getDpDv() {
		return dpdv;
	}

	float DifferentialGeometry::getArea() const {
		return area;
	}

	const Material * DifferentialGeometry::getMaterialPtr() const {
		return matPtr;
	}


}


