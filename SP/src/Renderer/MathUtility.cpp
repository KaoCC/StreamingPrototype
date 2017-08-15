#include "MathUtility.hpp"



namespace SP {

	RadeonRays::float3 getOrthogonalVector(const RadeonRays::float3& n) {

		// Note: p dot n == 0
		RadeonRays::float3 p;


		if (std::abs(n.z) > 0.f) {	// let x = 0
			float len = std::sqrt(n.y * n.y + n.z * n.z);
			p.x = 0.f;
			p.y = -n.z / len;
			p.z = n.y / len;

		} else {	// let z = 0
			float len = std::sqrt(n.x * n.x + n.y * n.y);
			p.x = n.y / len;
			p.y = -n.x / len;
			p.z = 0.f;

		}


		return p;

	}

	float balanceHeuristic(int nf, float fpdf, int ng, float gpdf) {
		float f = nf * fpdf;
		float g = ng * gpdf;
		return (f) / (f + g);
	}

	RadeonRays::matrix matrix_from_rows3(RadeonRays::float3 c0, RadeonRays::float3 c1, RadeonRays::float3 c2) {

		RadeonRays::matrix m;
		m.m00 = c0.x; m.m01 = c0.y; m.m02 = c0.z; m.m03 = 0.f;

		m.m10 = c1.x; m.m11 = c1.y; m.m12 = c1.z; m.m13 = 0.f;

		m.m20 = c2.x; m.m21 = c2.y; m.m22 = c2.z; m.m23 = 0.f;

		m.m30 = 0.f; m.m31 = 0.f; m.m32 = 0.f; m.m33 = 1.f;

		return m;

	}

	RadeonRays::matrix matrix_from_cols3(RadeonRays::float3 c0, RadeonRays::float3 c1, RadeonRays::float3 c2) {
		RadeonRays::matrix m;

		m.m00 = c0.x; m.m01 = c1.x; m.m02 = c2.x; m.m03 = 0.f;

		m.m10 = c0.y; m.m11 = c1.y; m.m12 = c2.y; m.m13 = 0.f;

		m.m20 = c0.z; m.m21 = c1.z; m.m22 = c2.z; m.m23 = 0.f;

		m.m30 = 0.f; m.m31 = 0.f; m.m32 = 0.f; m.m33 = 1.f;



		return m;
	}


}
