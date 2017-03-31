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

	private:

		// world position
		RadeonRays::float3 pos;

		// normal
		RadeonRays::float3 normal;

		// geo normal
		RadeonRays::float3 ng;

		// UVs
		RadeonRays::float3 uv;

		
		// Derivatives
		RadeonRays::float3 dpdu;
		RadeonRays::float3 dpdv;

		// area
		float area;


		RadeonRays::matrix worldToTangent;
		RadeonRays::matrix tangentToWorld;

		// Mat ?
		Material mat;
	};




}
















#endif

