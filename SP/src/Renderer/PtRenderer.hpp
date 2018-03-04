#ifndef _SP_PTRENDERER_HPP_
#define _SP_PTRENDERER_HPP_

#include "Renderer.hpp"
#include "SceneTracker.hpp"
#include "RenderOutput.hpp"

#include <vector>
#include <memory>

#include "radeon_rays.h"

#include "Path.hpp"
#include "ApiEngine.hpp"


namespace SP {



	class PtRenderer : public Renderer {


	public:
		PtRenderer(unsigned num_bounces, ApiEngine& engine);


		// Inherited via Renderer
		//virtual std::shared_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const override;

		//virtual void deleteOutput(Output * output) const override;

		//virtual void clear(RadeonRays::float3 const & val, RenderOutput & output) const override;

		virtual void preprocess(Scene const & scene) override;

		virtual void render(Scene const & scene, size_t configIdx, const RenderingTask& renderingTask) override;

		virtual void setOutput(std::shared_ptr<RenderOutput> output) override;

		virtual void renderDepthMap(Scene const& scene, size_t configIdx, const RenderingTask& renderingTask) override;


		struct RenderData {
			// host buffers
			std::vector<RadeonRays::ray> host_rays[2];
			std::vector<int> host_hits;

			std::vector<int> host_pixelIndex[2];
			std::vector<int> host_compactedIndex;
			std::vector<int> host_iota;

			std::vector<RadeonRays::ray> host_shadowrays;
			std::vector<int> host_shadowhits;

			std::vector<RadeonRays::Intersection> host_intersections;
			int host_hitcount;


			std::vector<Path> host_path;

			std::vector<RadeonRays::float3> host_lightSamples;


		};


		struct PathState;


		~PtRenderer();


	protected:

		// Generate rays
		void generatePrimaryRays(const Scene& scene, size_t camIdx);

		void resizeWorkingSet(const Output& out);


		// Shade first hit
		void shadeSurface(const Scene& scene, unsigned pass);
		// Evaluate volume
		void evaluateVolume(unsigned pass);
		// Handle missing rays
		void shadeMiss(unsigned pass);
		// Gather light samples and account for visibility
		void gatherLightSamples(unsigned pass);
		// Restore pixel indices after compaction
		void restorePixelIndices(unsigned pass);
		// Convert intersection info to compaction predicate
		void filterPathStream(unsigned pass);
		// Integrate volume
		void shadeVolume(unsigned pass);
		// Shade background
		void shadeBackground(unsigned pass);

		// Depth
		void generateDepthRays(const Scene& scene, size_t camIdx);
		void computeDepthMap(const Scene& scene, size_t camIdx);

		// Helper
		void compactIndex();
		void scanExclusiveAdd(std::vector<int>& addr);


	private:

		// Output 
		std::shared_ptr<RenderOutput> renderOutPtr;


		//SceneTracker sceneTracker;

		RenderData renderData;

		//std::vector<RadeonRays::Shape*> shapes;

		// KAOCC: add ref to ApiEngine
		//std::unique_ptr<ApiEngine>& mEngineRef;

		ApiEngine& mEngineRef;

		unsigned numOfBounces;
		std::uint32_t mFrameCount = 0;
	};

}







#endif



