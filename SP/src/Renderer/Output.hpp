#ifndef _SP_OUTPUT_HPP_
#define _SP_OUTPUT_HPP_

#include "math/float3.h"
#include <cstdint>

namespace SP {

	class Output {

	public:

		Output(std::uint32_t w, std::uint32_t h);

		virtual ~Output() = default;


		//brief Interface for rendering output.
		//Represents discrete 2D surface with [0..w]x[0..h] coordinate ranges.
		virtual void getData(RadeonRays::float3* data) const = 0;

		// Get width
		std::uint32_t getWidth() const;
		// Get height
		std::uint32_t getHeight() const;

	private:
		std::uint32_t width;
		std::uint32_t height;
	};


}


#endif


