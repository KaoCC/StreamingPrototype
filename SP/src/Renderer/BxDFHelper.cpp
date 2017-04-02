#include "BxDFHelper.hpp"


namespace SP {

	bool BxDFHelper::isEmissive(const Material * matPtr) {
		return matPtr->hasEmission();
	}

	bool BxDFHelper::isSingular(const Material * matPtr) {

		auto sinMatPtr{ dynamic_cast<const SingleBxDF*>(matPtr) };

		if (sinMatPtr) {

			auto type{ sinMatPtr->getBxDFType() };
			return type == SingleBxDF::BxDFType::kIdealReflect || type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough;

		} else {		// mixed
			return false;
		}


	}

	bool BxDFHelper::isBTDF(const Material * matPtr) {
		auto sinMatPtr{ dynamic_cast<const SingleBxDF*>(matPtr) };

		if (sinMatPtr) {

			auto type{ sinMatPtr->getBxDFType() };
			return type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough || type == SingleBxDF::BxDFType::kTranslucent
				|| type == SingleBxDF::BxDFType::kMicrofacetRefractionGGX || type == SingleBxDF::BxDFType::kMicrofacetRefractionBeckmann;

		} else {	// mixed
			return false;
		}
	}


	RadeonRays::float3 BxDFHelper::sampleBxDF(const DifferentialGeometry & diffGeo, RadeonRays::float3 wi, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) {

		RadeonRays::float3 result = 0.f;

		auto* singleBxDFPtr = dynamic_cast<const SingleBxDF*>(diffGeo.getMaterialPtr());

		if (singleBxDFPtr) {

			switch (singleBxDFPtr->getBxDFType()) {
			case SingleBxDF::BxDFType::kLambert:
				result = sampleLambert(diffGeo, wi, sample, wo, pdf);
				break;
			default:		// Error ?
				pdf = 0.f;
				break;
			}

		} else {
			throw std::runtime_error("Not a single BxDF instance");
		}


		return result;

	}

	RadeonRays::float3 BxDFHelper::sampleLambert(const DifferentialGeometry & diffGeo, RadeonRays::float3 wi, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) {


		//const RadeonRays::float3 kd = Texture_GetValue3f(dg->mat.kx.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.kxmapidx));

		// TMP ! 
		const RadeonRays::float3 kd = diffGeo.getMaterialPtr()->getInputValue("albedo").floatValue;

		//		wo = Sample_MapToHemisphere(sample, make_float3(0.f, 1.f, 0.f), 1.f);


		//float F = diffGeo.tmpFresnel;		// always 1
		float F = 1.0f;

		pdf = fabs(wo.y) / PI;

		// check value
		return F * kd * (1 / PI);
	}

	RadeonRays::float3 BxDFHelper::getEmissiveLe(const DifferentialGeometry & diffGeo) {

		//const RadeonRays::float3 kd = Texture_GetValue3f(dg->mat.kx.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.kxmapidx));

		// TMP ! 
		const RadeonRays::float3 kd = diffGeo.getMaterialPtr()->getInputValue("albedo").floatValue;

		return kd;

	}









}


