#ifndef _SP_BXDFHELPER_HPP_
#define _SP_BXDFHELPER_HPP_

//#include "Scene/Material.hpp"

#include "radeon_rays.h"

namespace SP {

	class Material;
	class DifferentialGeometry;


	class BxDFHelper {

	public:

		// helper function for BxDF type check
		static bool isEmissive(const Material* matPtr);
		static bool isSingular(const Material* matPtr);
		static bool isBTDF(const Material* matPtr);


		// helper function 
		static RadeonRays::float3 sample(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf);

		static RadeonRays::float3 sampleLambert(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf);
		static RadeonRays::float3 sampleIdealReflect(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf);


		// emissive
		static RadeonRays::float3 getEmissiveLe(const DifferentialGeometry& diffGeo);

		static RadeonRays::float3 evaluate(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi,  const RadeonRays::float3& wo);
		static RadeonRays::float3 evaluateLambert(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi, const RadeonRays::float3& wo);

		// PDF !
		static float getPDF(const DifferentialGeometry& diffGeo, const RadeonRays::float3& wi, const RadeonRays::float3& wo);

		BxDFHelper() = delete;
		~BxDFHelper() = delete;

	};






}





















#endif

