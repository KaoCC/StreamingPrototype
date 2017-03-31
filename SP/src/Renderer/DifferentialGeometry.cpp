#include "DifferentialGeometry.hpp"

#include "Scene/Shape.hpp"

void SP::DifferentialGeometry::fill(const RadeonRays::Intersection & isectRef, const std::vector<const Mesh*>& meshPtrs) {

	int shapeId = isectRef.shapeid - 1;			//CHECK !
	int primId = isectRef.primid;								// CHECK !!!

	RadeonRays::float2 uv;
	uv.x = isectRef.uvwt.x;
	uv.y = isectRef.uvwt.y;


	// shape
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

	RadeonRays::matrix matrixI;			// I

	normal = RadeonRays::normalize(RadeonRays::transform_vector((1.f - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2, matrixI));  //CHECK THIS !
	pos = RadeonRays::transform_point((1.f - uv.x - uv.y) * v0 + uv.x * v1 + uv.y * v2, matrixI);






}
