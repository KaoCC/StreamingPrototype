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


		return RadeonRays::normalize(p);

	}

}
