#ifndef _SP_RENDEROUTPUT_HPP_
#define _SP_RENDEROUTPUT_HPP_

#include "Output.hpp"
#include <vector>

namespace SP {

	
	class RenderOutput : public Output {

	public:

		RenderOutput(std::size_t w, std::size_t h);

		virtual ~RenderOutput() = default;

		//virtual void getData(RadeonRays::float3 * dt) const override;

		// tmp
		std::vector<RadeonRays::float3> copyData() const;

		//tmp
		//std::vector<RadeonRays::float3>& getInternalStorage();


		// export operators
		RadeonRays::float3& operator[](std::size_t pos);
		const RadeonRays::float3& operator[](std::size_t pos) const;


	private:



		// Inherited via Output
		virtual std::size_t getStorageSize() const override;

		virtual void resizeStorage(std::size_t newSize) override;

		virtual void resetStorageDefault() override;



		std::vector<RadeonRays::float3> data;

	};











}









#endif

