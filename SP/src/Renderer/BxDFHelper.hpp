#ifndef _SP_BXDFHELPER_HPP_
#define _SP_BXDFHELPER_HPP_

#include "Scene/Material.hpp"

namespace SP {




	class BxDFHelper {

	public:

		// helper function for BxDF type check
		static bool isEmissive(const Material* matPtr);
		static bool isSingular(const Material* matPtr);
		static bool isBTDF(const Material* matPtr);


		// helper function 
		static RadeonRays::float3 sampleBxDF(const DifferentialGeometry& diffGeo, RadeonRays::float3 wi, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf);

		static RadeonRays::float3 sampleLambert(const DifferentialGeometry& diffGeo, RadeonRays::float3 wi, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf);

		// emissive
		static RadeonRays::float3 getEmissiveLe(const DifferentialGeometry& diffGeo);


		BxDFHelper() = delete;
		~BxDFHelper() = delete;

	};






}





















#endif

