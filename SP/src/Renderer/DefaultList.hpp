#ifndef _SP_DEFAULTLIST_HPP_
#define _SP_DEFAULTLIST_HPP_


#include <memory>


#include "Scene/Shape.hpp"


#include "math/matrix.h"


namespace SP {


	enum class DefaultShapeType {
		kTriangle,
		kSquare,
		kBudda		// KAOCC: not really a shape ...
	};



    class Triangle : public Mesh {
	public:
		Triangle();
    };


	class Square : public Mesh {
	public:
		Square();
	};

	class Budda : public Mesh {
	public:
		Budda();
	};


	std::unique_ptr<Mesh> createDefaultShape(DefaultShapeType type);

}


#endif

