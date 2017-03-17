#ifndef _SP_LIGHT_HPP_
#define _SP_LIGHT_HPP_

#include "SceneObject.hpp"
#include "Shape.hpp"

namespace SP {

	class Light : public SceneObject {
	public:
		Light();
		virtual ~Light() = default;

		// Set & Get




	private:

		// pos

		// dir

		// RADIANCE !

	};


	class AreaLight : public Light {
	public:

		AreaLight(Shape const* shape, size_t idx);


		// get shape

		// get index

	private:

		// Parent shape

		// Parent primitive index

		size_t primitiveIndex;
	};

}




#endif



