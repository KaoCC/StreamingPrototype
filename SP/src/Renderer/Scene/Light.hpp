#ifndef _SP_LIGHT_HPP_
#define _SP_LIGHT_HPP_

#include "SceneObject.hpp"
#include "Shape.hpp"

#include "../DifferentialGeometry.hpp"

namespace SP {

	class Light : public SceneObject {
	public:
		Light();
		virtual ~Light() = default;

		// Set & Get
		RadeonRays::float3 getPosition() const;
		void setPosition(const RadeonRays::float3& p);

		RadeonRays::float3 getDirection() const;
		void setDirection(const RadeonRays::float3& d);

		// Set and get emitted radiance (differential)
		RadeonRays::float3 getRadiance() const;
		void setRadiance(const RadeonRays::float3& e);


		// sampling ?
		virtual RadeonRays::float3 sample(const DifferentialGeometry& diffGeo, RadeonRays::float2 sample, RadeonRays::float3& wo, float& pdf) const = 0;
		// true if: Point, Spot, Directional. Otherwise return false.
		virtual bool isSingular() const  = 0;


	private:
		// position
		RadeonRays::float3 pos;
		// direction
		RadeonRays::float3 dir;
		// RADIANCE !
		RadeonRays::float3 radiance;
	};


	class AreaLight : public Light {
	public:

		AreaLight(Shape const* sh, size_t idx);

		// get shape
		const Shape* getShape() const;
		// get index
		size_t getPrimitiveIndex() const;

		// Inherited via Light
		virtual RadeonRays::float3 sample(const DifferentialGeometry & diffGeo, RadeonRays::float2 sample, RadeonRays::float3 & wo, float & pdf) const override;
		virtual bool isSingular() const override;


	private:
		// Parent shape
		Shape const* shape;
		// Parent primitive index
		size_t primitiveIndex;


	};

}




#endif



