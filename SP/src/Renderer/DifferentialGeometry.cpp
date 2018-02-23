#include "DifferentialGeometry.hpp"


#include "MathUtility.hpp"


namespace SP {

	const RadeonRays::matrix  DifferentialGeometry::matrixI {};

	DifferentialGeometry::DifferentialGeometry(const RadeonRays::Intersection& isectRef, const Scene& scene) {

		int shapeId = isectRef.shapeid - 1;            //CHECK !
		int primId = isectRef.primid;                                // CHECK !!!

		RadeonRays::float2 localUV;                        // check this !
		localUV.x = isectRef.uvwt.x;
		localUV.y = isectRef.uvwt.y;


		// shape (Mesh)
		const Mesh& meshData = dynamic_cast<const Mesh&>(scene.getShape(shapeId));

		const uint32_t* indexArray = meshData.getIndices();

		// This is the core of this function !
		// if this goes wrong then ... boom !
		const uint32_t& i0 = indexArray[3 * primId];                                // CHECK ?
		const uint32_t& i1 = indexArray[3 * primId + 1];
		const uint32_t& i2 = indexArray[3 * primId + 2];

		const RadeonRays::float3* normalArray = meshData.getNormals();

		const RadeonRays::float3& n0 = normalArray[i0];
		const RadeonRays::float3& n1 = normalArray[i1];
		const RadeonRays::float3& n2 = normalArray[i2];


		const RadeonRays::float3* verticeArray = meshData.getVertices();

		const RadeonRays::float3& v0 = verticeArray[i0];
		const RadeonRays::float3& v1 = verticeArray[i1];
		const RadeonRays::float3& v2 = verticeArray[i2];


		const RadeonRays::float2* uvArray = meshData.getUVs();

		const RadeonRays::float2& uv0 = uvArray[i0];
		const RadeonRays::float2& uv1 = uvArray[i1];
		const RadeonRays::float2& uv2 = uvArray[i2];

		//RadeonRays::matrix matrixI;			// I

		// normal, position, uv, ng (barycentric)
		normal = RadeonRays::normalize(
				RadeonRays::transform_vector((1.f - localUV.x - localUV.y) * n0 + localUV.x * n1 + localUV.y * n2, matrixI));  //CHECK THIS !
		pos = RadeonRays::transform_point((1.f - localUV.x - localUV.y) * v0 + localUV.x * v1 + localUV.y * v2, matrixI);
		uv = (1.f - localUV.x - localUV.y) * uv0 + localUV.x * uv1 + localUV.y * uv2;
		normalGeo = RadeonRays::normalize(RadeonRays::cross(v1 - v0, v2 - v0));


		if (RadeonRays::dot(normalGeo, normal) < 0) {
			normalGeo = -normalGeo;
		}

		// Material ?
		matPtr = &(meshData.getMaterial());

		//workaround
		currentMat = matPtr;

		//float du1 = uv0.x - uv2.x;
		//float du2 = uv1.x - uv2.x;
		//float dv1 = uv0.y - uv2.y;
		//float dv2 = uv1.y - uv2.y;

		//RadeonRays::float3 dp1 = v0 - v2;
		//RadeonRays::float3 dp2 = v1 - v2;

		//float determin = du1 * dv2 - dv1 * du2;

		// dpdu ???
		dpdu = RadeonRays::normalize(getOrthogonalVector(normal));

		// dpdv
		dpdv = RadeonRays::normalize(RadeonRays::cross(normal, dpdu));

		// For area calculation
		const RadeonRays::float3& p0 = RadeonRays::transform_point(v0, matrixI);
		const RadeonRays::float3& p1 = RadeonRays::transform_point(v1, matrixI);
		const RadeonRays::float3& p2 = RadeonRays::transform_point(v2, matrixI);


		area = 0.5f * std::sqrt(RadeonRays::cross(p2 - p0, p2 - p1).sqnorm());   // check the value !

	}



	void DifferentialGeometry::calculateTangentTransform() {

		worldToTangent = matrix_from_rows3(dpdu, normal, dpdv);

		// check this value !
		tangentToWorld = worldToTangent.transpose();

		worldToTangent.m03 = -RadeonRays::dot(dpdu, pos);
		worldToTangent.m13 = -RadeonRays::dot(normal, pos);
		worldToTangent.m23 = -RadeonRays::dot(dpdv, pos);


		tangentToWorld.m03 = pos.x;
		tangentToWorld.m13 = pos.y;
		tangentToWorld.m23 = pos.z;

	}

	//RadeonRays::float3& DifferentialGeometry::getPosition() {
	//	return pos;
	//}

	const RadeonRays::float3& DifferentialGeometry::getPosition() const {
		return pos;
	}

	const RadeonRays::float3& DifferentialGeometry::getNormal() const {
		return normal;
	}

	void DifferentialGeometry::setNormal(const RadeonRays::float3& n) {
		normal = n;
	}

	const RadeonRays::float3& DifferentialGeometry::getNormalGeo() const {
		return normalGeo;
	}

	void DifferentialGeometry::setNormalGeo(const RadeonRays::float3 & ng) {
		normalGeo = ng;
	}

	const RadeonRays::float2& DifferentialGeometry::getUV() const{
		return uv;
	}

	void DifferentialGeometry::setUV(const RadeonRays::float2 & newVal) {
		uv = newVal;
	}

	const RadeonRays::float3& DifferentialGeometry::getDpDu() const {
		return dpdu;
	}

	void DifferentialGeometry::setDpDu(const RadeonRays::float3 & newVal) {
		dpdu = newVal;
	}

	const RadeonRays::float3& DifferentialGeometry::getDpDv() const {
		return dpdv;
	}

	void DifferentialGeometry::setDpDv(const RadeonRays::float3 & newVal) {
		dpdv = newVal;
	}

	float DifferentialGeometry::getArea() const {
		return area;
	}

	const Material * DifferentialGeometry::getOriginalMaterial() const {
		return matPtr;
	}

	const Material* DifferentialGeometry::getCurrentMaterial() const {
		return currentMat;
	}

	void DifferentialGeometry::setCurrentMaterial(const Material * mat) {
		currentMat = mat;
	}

	const RadeonRays::matrix& DifferentialGeometry::getWorldToTangentMatrix() const {
		return worldToTangent;
	}

	const RadeonRays::matrix& DifferentialGeometry::getTangentToWorldMatrix() const {
		return tangentToWorld;
	}


}


