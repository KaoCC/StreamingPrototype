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
		DifferentialGeometry() {} // tmp, for empty value initialization, should not use directly...
		DifferentialGeometry(const RadeonRays::Intersection& isectRef, const Scene& scene);

		//void fill(const RadeonRays::Intersection& isectRef, const Scene& scene);

		void calculateTangentTransform();

		//get Ref
		//RadeonRays::float3& getPosition();
		const RadeonRays::float3& getPosition() const;

		const RadeonRays::float3& getNormal() const;
		void setNormal(const RadeonRays::float3&);

		const RadeonRays::float3& getNormalGeo() const;
		void setNormalGeo(const RadeonRays::float3&);

		const RadeonRays::float2& getUV() const;
		void setUV(const RadeonRays::float2&);

		const RadeonRays::float3& getDpDu() const;
		void setDpDu(const RadeonRays::float3&);

		const RadeonRays::float3& getDpDv() const;
		void setDpDv(const RadeonRays::float3&);

		float getArea() const;

		// tmp workaround
		const Material* getOriginalMaterial() const;

		const Material* getCurrentMaterial() const;

		// tmp workaround
		void setCurrentMaterial(const Material* mat);


		const RadeonRays::matrix& getWorldToTangentMatrix() const;
		const RadeonRays::matrix& getTangentToWorldMatrix() const;

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
		
		// tmp workaroind for mat selection
		const Material* currentMat = nullptr;

		// tmp workaround
		float tmpFresnel = 1.0f;

		static const RadeonRays::matrix matrixI;

	};




}
















#endif

