#ifndef _SP_MATHUTILITY_HPP_
#define _SP_MATHUTILITY_HPP_


#include "math/float3.h"
#include "math/matrix.h"


namespace SP {

	RadeonRays::float3 getOrthogonalVector(const RadeonRays::float3& n);


	// for MIS
	float balanceHeuristic(int nf, float fpdf, int ng, float gpdf);



	// matrix
	RadeonRays::matrix matrix_from_rows3(RadeonRays::float3 c0, RadeonRays::float3 c1, RadeonRays::float3 c2);
	RadeonRays::matrix matrix_from_cols3(RadeonRays::float3 c0, RadeonRays::float3 c1, RadeonRays::float3 c2);

}











#endif

