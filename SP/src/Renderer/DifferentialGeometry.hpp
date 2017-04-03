#ifndef _SP_DIFFERENTIALGEOMETRY_HPP_
#define _SP_DIFFERENTIALGEOMETRY_HPP_


//#include "math/float3.h"
//#include "math/matrix.h"

#include "Scene/Material.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Shape.hpp"
#include "radeon_rays.h"

namespace SP {



	class DifferentialGeometry {

	public:
	//	DifferentialGeometry();

		void fill(const RadeonRays::Intersection& isectRef, const std::vector<const Mesh*>& meshPtrs);

		//get Ref
		RadeonRays::float3& getPosition();
		RadeonRays::float3 getPosition() const;

		RadeonRays::float3& getNormal();
		RadeonRays::float3& getNormalGeo();
		RadeonRays::float2& getUV();

		RadeonRays::float3& getDpDu();
		RadeonRays::float3& getDpDv();

		float getArea() const;
		const Material* getMaterialPtr() const;

	private:

		// world position
		RadeonRays::float3 pos;

		// normal
		RadeonRays::float3 normal;

		// geo normal
		RadeonRays::float3 normalGeo;

		// UVs
		RadeonRays::float2 uv;

		
		// Derivatives
		RadeonRays::float3 dpdu;
		RadeonRays::float3 dpdv;

		// area
		float area;


		RadeonRays::matrix worldToTangent;
		RadeonRays::matrix tangentToWorld;

		// Mat ?
		const Material* matPtr = nullptr;

		// tmp workaround
		float tmpFresnel = 1.0f;
	};




}
















#endif

