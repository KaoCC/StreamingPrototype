#ifndef _SP_PTRENDERER_HPP_
#define _SP_PTRENDERER_HPP_

#include "Renderer.hpp"
#include "SceneTracker.hpp"
#include "RenderOutput.hpp"

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

		virtual void render(Scene const & scene, size_t configIdx) override;

		virtual void setOutput(Output * output) override;



		struct RenderData;
		struct PathState;


	protected:

		// Generate rays
		void generatePrimaryRays(const Scene& scene, size_t camIdx);

		void resizeWorkingSet(const Output& out);


		// Shade first hit
		void shadeSurface(int pass);
		// Evaluate volume
		void evaluateVolume(int pass);
		// Handle missing rays
		void shadeMiss(int pass);
		// Gather light samples and account for visibility
		void gatherLightSamples(int pass);
		// Restore pixel indices after compaction
		void restorePixelIndices(int pass);
		// Convert intersection info to compaction predicate
		void filterPathStream(int pass);
		// Integrate volume
		void shadeVolume(int pass);
		// Shade background
		void shadeBackground(int pass);


		// Helper
		void compactIndex();
		void scanExclusiveAdd(std::vector<int>& addr);


	private:

		// Output 
		// KAOCC: use shared pointer ???
		RenderOutput* renderOutPtr  = nullptr;


		SceneTracker sceneTracker;

		std::unique_ptr<RenderData> renderData;

		//std::vector<RadeonRays::Shape*> shapes;


		std::uint32_t numOfBounces;
		std::uint32_t frameCount = 0;
	};

}







#endif



