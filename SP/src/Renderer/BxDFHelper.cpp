#include "BxDFHelper.hpp"


#include "Sampler.hpp"

#include "Renderer\Scene\Texture.hpp"
#include "DifferentialGeometry.hpp"


// test
#include <iostream>

namespace SP {


	static RadeonRays::float3 getValue(const DifferentialGeometry & diffGeo) {

		const auto& matPtr = diffGeo.getCurrentMaterial();

		// FIXME: check for single BxDF

		const auto& val = matPtr->getInputValue("albedo");

		if (val.type == Material::InputType::kFloat4) {

			return val.floatValue;

		} else if (val.type == Material::InputType::kTexture) {

			// check value, might be wrong
			return val.texValue->sample2D(diffGeo.getUV());

		} else {
			// error ?
			throw std::runtime_error("getValue: unsupported input type");

		}

	}

	bool BxDFHelper::isEmissive(const Material * matPtr) {
		return matPtr->hasEmission();
	}

	bool BxDFHelper::isSingular(const Material * matPtr) {

		auto sinMatPtr =  dynamic_cast<const SingleBxDF*>(matPtr);

		if (sinMatPtr != nullptr) {

			auto type = sinMatPtr->getBxDFType() ;
			return type == SingleBxDF::BxDFType::kIdealReflect || type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough;

		}
		// mixed
		
		return false;


	}

	bool BxDFHelper::isBTDF(const Material * matPtr) {
		auto sinMatPtr =  dynamic_cast<const SingleBxDF*>(matPtr);

		if (sinMatPtr != nullptr) {

			auto type = sinMatPtr->getBxDFType();
			return type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough || type == SingleBxDF::BxDFType::kTranslucent
				|| type == SingleBxDF::BxDFType::kMicrofacetRefractionGGX || type == SingleBxDF::BxDFType::kMicrofacetRefractionBeckmann;

		}
		// mixed
			return false;

	}


	RadeonRays::float3 BxDFHelper::sample(const DifferentialGeometry & diffGeo, const RadeonRays::float3& wi, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) {

		RadeonRays::float3 result = 0.f;

		auto* singleBxDFPtr = dynamic_cast<const SingleBxDF*>(diffGeo.getCurrentMaterial());

		const RadeonRays::float3& wi_tang = diffGeo.getWorldToTangentMatrix() * wi;
		RadeonRays::float3 wo_tang;

		if (singleBxDFPtr != nullptr) {

			switch (singleBxDFPtr->getBxDFType()) {
			case SingleBxDF::BxDFType::kLambert:
				result = sampleLambert(diffGeo, wi_tang, sample, wo_tang, pdf);
				break;
			case SingleBxDF::BxDFType::kIdealReflect:
				result = sampleIdealReflect(diffGeo, wi_tang, sample, wo_tang, pdf);
				break;
			default:		// Error ?
				pdf = 0.f;

				throw std::runtime_error("sample: unsupported type");

				break;
			}

		} else {
			throw std::runtime_error("Not a single BxDF instance");
		}


		wo = diffGeo.getTangentToWorldMatrix() * wo_tang;

		return result;

	}

	RadeonRays::float3 BxDFHelper::sampleLambert(const DifferentialGeometry & diffGeo, const RadeonRays::float3& wi, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) {


		//const RadeonRays::float3 kd = Texture_GetValue3f(dg->mat.kx.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.kxmapidx));

		// TMP ! 
		//const RadeonRays::float3& kd = diffGeo.getCurrentMaterial()->getInputValue("albedo").floatValue;

		const RadeonRays::float3& kd = getValue(diffGeo);

//		std::cerr << "wo.y before: " << wo.y << std::endl;
//		std::cerr << "Sample: " << sample.x << " " << sample.y << '\n';

		wo = Sample_MapToHemisphere(sample, RadeonRays::float3(0.f, 1.f, 0.f), 1.f);

		//float F = diffGeo.tmpFresnel;		// always 1
		float F = 1.0f;

//		std::cerr << "wo.y: " << wo.y << std::endl;

		pdf = std::abs(wo.y) / PI;

		// check value
		return F * kd * (1 / PI);
	}

	RadeonRays::float3 BxDFHelper::sampleIdealReflect(const DifferentialGeometry & diffGeo, const RadeonRays::float3 & wi, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) {


		// TMP !!
		const RadeonRays::float3& ks = diffGeo.getCurrentMaterial()->getInputValue("albedo").floatValue;

		// Mirror reflect wi
		wo = RadeonRays::normalize(RadeonRays::float3(-wi.x, wi.y, -wi.z));

		// set pdf to 1 (pdf is infinite at that point, but deltas will cancel out while evaluating)
		pdf = 1.f;

		// check this value !
		float F = 1;		// diffGeo.fresnel

		float cosine_wo = std::abs(wo.y);
		
		// return reflectance value
		return cosine_wo > 0.f ? (F * ks * (1.f / cosine_wo)) : 0.f;
	}

	RadeonRays::float3 BxDFHelper::getEmissiveLe(const DifferentialGeometry & diffGeo) {

		//const RadeonRays::float3 kd = Texture_GetValue3f(dg->mat.kx.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.kxmapidx));

		// TMP ! 
		return diffGeo.getCurrentMaterial()->getInputValue("albedo").floatValue;
	}

	RadeonRays::float3 BxDFHelper::evaluate(const DifferentialGeometry & diffGeo, const RadeonRays::float3 & wi, const RadeonRays::float3 & wo) {

		RadeonRays::float3 result = 0.f;

		auto* singleBxDFPtr = dynamic_cast<const SingleBxDF*>(diffGeo.getCurrentMaterial());

		const RadeonRays::float3& wi_tang = diffGeo.getWorldToTangentMatrix() * wi;
		RadeonRays::float3 wo_tang = diffGeo.getWorldToTangentMatrix() * wo;

		if (singleBxDFPtr != nullptr) {

			switch (singleBxDFPtr->getBxDFType()) {
			case SingleBxDF::BxDFType::kLambert:
				result = evaluateLambert(diffGeo, wi_tang, wo_tang);
				break;
			default:		// Error ?
				//pdf = 0.f;
				throw std::runtime_error("Evaluate: unsupported type !");
				break;
			}

		}

		return result;
	}

	RadeonRays::float3 BxDFHelper::evaluateLambert(const DifferentialGeometry & diffGeo, const RadeonRays::float3 & wi, const RadeonRays::float3 & wo) {

		// tangent sapce

		// tmp !
		//const RadeonRays::float3& kd = diffGeo.getCurrentMaterial()->getInputValue("albedo").floatValue;

		const RadeonRays::float3& kd = getValue(diffGeo);

		//float F = dg->mat.fresnel;
		float F = 1.0;	// tmp

		return F * kd * (1 / PI);

	}









}


