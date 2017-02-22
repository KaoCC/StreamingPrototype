#ifndef _SP_PTRENDERER_HPP_
#define _SP_PTRENDERER_HPP_

#include "Renderer.hpp"
#include "SceneTracker.hpp"

#include <vector>
#include <memory>

#include "radeon_rays.h"

namespace SP {



	class PtRenderer : public Renderer {


	public:

		PtRenderer(int devidx, int num_bounces);



		// Inherited via Renderer
		virtual Output * createOutput(std::uint32_t w, std::uint32_t h) const override;

		virtual void deleteOutput(Output * output) const override;

		virtual void clear(RadeonRays::float3 const & val, Output & output) const override;

		virtual void preprocess(Scene const & scene) override;

		virtual void render(Scene const & scene) override;

		virtual void setOutput(Output * output) override;



		struct RenderData;


	protected:

		// Generate rays
		void generatePrimaryRays();

	private:

		SceneTracker sceneTracker;

		std::unique_ptr<RenderData> renderData;

		//std::vector<RadeonRays::Shape*> shapes;


		std::uint32_t numOfBounces;
		std::uint32_t frameCount = 0;
	};

}







#endif



