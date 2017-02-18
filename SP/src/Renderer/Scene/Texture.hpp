#ifndef _SP_TEXTURE_HPP_
#define _SP_TEXTURE_HPP_

#include "SceneObject.hpp"

namespace SP {



	class Texture : public SceneObject {


	public:
		enum class Format {
			kRGBA8,
			kRGBA16,
			kRGBA32
		};



	private:

		Format format;

	};




}











#endif

