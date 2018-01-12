#ifndef _SP_OUTPUT_HPP_
#define _SP_OUTPUT_HPP_

#include "math/float3.h"
#include <cstdint>

namespace SP {

	class Output {

	public:

		Output(std::size_t w, std::size_t h);

		virtual ~Output() = default;


		//brief Interface for rendering output.
		//Represents discrete 2D surface with [0..w]x[0..h] coordinate ranges.
		//virtual void getData(RadeonRays::float3* data) const = 0;

		

		// Get width
		std::size_t getWidth() const;
		// Get height
		std::size_t getHeight() const;


		std::size_t getSize() const;
		void resize(std::size_t w, std::size_t h);
		void resetToDefault();


	private:

		virtual std::size_t getStorageSize() const = 0;
		virtual void resizeStorage(std::size_t newSize) = 0;
		virtual void resetStorageDefault() = 0;

		std::size_t width;
		std::size_t height;
	};


}


#endif


