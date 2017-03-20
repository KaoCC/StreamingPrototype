#include "RenderOutput.hpp"

namespace SP {
	RenderOutput::RenderOutput(std::uint32_t w, std::uint32_t h) : Output(w, h), data(w * h) {


	}
	void RenderOutput::getData(RadeonRays::float3 * data) const {

		//KAOCC: copy or pass pointer ?

		
		throw std::runtime_error("Yet to be done");
	}

	std::vector<RadeonRays::float3> RenderOutput::copyData() {
		return data;
	}

}
