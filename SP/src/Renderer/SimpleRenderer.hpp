#ifndef _SP_SIMPLERENDERER_HPP_
#define _SP_SIMPLERENDERER_HPP_

#include <memory>

#include "Renderer.hpp"
#include "ApiEngine.hpp"

namespace SP {

	class SimpleRenderer : public Renderer {

	public:

		SimpleRenderer(std::unique_ptr<ApiEngine>& engine);


		// Inherited via Renderer
		virtual std::shared_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const override;

		//virtual void deleteOutput(Output * output) const override;

		virtual void clear(RadeonRays::float3 const & val, Output & output) const override;

		virtual void preprocess(Scene const & scene) override;

		virtual void render(Scene const & scene, size_t configIdx) override;

		virtual void setOutput(std::shared_ptr<Output>  output) override;

		~SimpleRenderer();

	private:

		struct SimpleRenderData;

		void generatePrimaryRays(const Scene& scene, size_t camIdx);
		void resizeWorkingSet(const Output& out);

		void simpleShading();

		std::unique_ptr<ApiEngine>& mEngineRef;
		std::unique_ptr<SimpleRenderData> mSimpleRenderDataPtr;

		std::shared_ptr<RenderOutput> mRenderOutPtr;


	};

}















#endif


