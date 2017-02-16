#include "PtRenderer.hpp"



namespace SP {



	struct PtRenderer::RenderData {

		// RadeonRays stuff
		RadeonRays::Buffer* rays[2];
		RadeonRays::Buffer* shadowrays = nullptr;
		RadeonRays::Buffer* shadowhits = nullptr;
		RadeonRays::Buffer* hits = nullptr;
		RadeonRays::Buffer* intersections = nullptr;
		RadeonRays::Buffer* hitcount = nullptr;				// types ?
	};




	PtRenderer::PtRenderer(int devidx, int num_bounces) : renderData(new RenderData), numOfBounces(num_bounces) {


		// Allocate space ?

	}

	Output * PtRenderer::createOutput(std::uint32_t w, std::uint32_t h) const {
		return nullptr;
	}

	void PtRenderer::deleteOutput(Output * output) const {
	}

	void PtRenderer::clear(RadeonRays::float3 const & val, Output & output) const {
	}

	void PtRenderer::preprocess(Scene const & scene) {
	}

	void PtRenderer::render(Scene const & scene) {
	}

	void PtRenderer::setOutput(Output * output) {
	}



}