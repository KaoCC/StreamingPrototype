#ifndef _SP_RENDEROUTPUT_HPP_
#define _SP_RENDEROUTPUT_HPP_

#include "Output.hpp"
#include <vector>

namespace SP {

	
	class RenderOutput : public Output {

	public:

		RenderOutput(std::uint32_t w, std::uint32_t h);

		virtual ~RenderOutput() = default;

		virtual void getData(RadeonRays::float3 * dt) const override;

		// tmp
		std::vector<RadeonRays::float3> copyData() const;

		//tmp
		std::vector<RadeonRays::float3>& getInternalStorage();

	private:

		std::vector<RadeonRays::float3> data;

	};











}









#endif

