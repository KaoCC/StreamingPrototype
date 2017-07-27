
#ifndef _SP_RENDERER_HPP_
#define _SP_RENDERER_HPP_


#include <cstdint>

#include "math/float3.h"
#include "math/int2.h"
#include "radeon_rays.h"


#include <memory>

namespace SP {

	class Output;
	class Scene;
	class PerspectiveCamera;

	class Renderer {

	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		/**
		\brief Create output of a given size.

		\param w Output surface width
		\param h Output surface height
		*/
		virtual std::shared_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const = 0;

		/**
		\brief Delete given output.

		\param output The output to delete
		*/
		//virtual void deleteOutput(Output* output) const = 0;

		/**
		\brief Clear output surface using given value.

		\param val Value to clear to
		\param output Output to clear
		*/
		virtual void clear(RadeonRays::float3 const& val, Output& output) const = 0;

		/**
		\brief Do necessary scene dependent computations and caching.

		\param scene The scene to process
		*/
		virtual void preprocess(Scene const& scene) = 0;

		/**
		\brief Render single iteration.

		\param scene Scene to render
		*/
		virtual void render(Scene const& scene, size_t configIdx) = 0;

		/**
		\brief Set the output for rendering.

		\param output The output to render into.
		*/
		virtual void setOutput(std::shared_ptr<Output> output) = 0;


		/**
		Forbidden stuff.
		*/
		Renderer(Renderer const&) = delete;
		Renderer& operator = (Renderer const&) = delete;



	};



	// helper function
	void generateRandomRay(uint32_t rngseed, uint32_t x, uint32_t y, uint32_t imageWidth, uint32_t imageHeight, RadeonRays::ray& currentRay, const PerspectiveCamera* cameraPtr);


}










#endif



