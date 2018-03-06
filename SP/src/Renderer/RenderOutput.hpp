#ifndef _SP_RENDEROUTPUT_HPP_
#define _SP_RENDEROUTPUT_HPP_

#include "Output.hpp"
#include <vector>
#include <mutex>

namespace SP {

	
	class RenderOutput : public Output {

	public:

		RenderOutput(std::size_t w, std::size_t h);

		virtual ~RenderOutput() = default;

		//virtual void getData(RadeonRays::float3 * dt) const override;

		// tmp
		std::vector<RadeonRays::float3> copyData() const;
		std::vector<float> copyDepthData() const;

		//tmp
		//std::vector<RadeonRays::float3>& getInternalStorage();


		// export operators
		RadeonRays::float3& operator[](std::size_t pos);
		const RadeonRays::float3& operator[](std::size_t pos) const;

		float& getDepthValue(std::size_t pos);
		const float& getDepthValue(std::size_t pos) const;

		std::vector<RadeonRays::float3>& getData();
		std::vector<float>& getDepthData();

		virtual void resetStorageDefault() override;

		std::mutex depthLock;
	private:



		// Inherited via Output
		virtual std::size_t getStorageSize() const override;

		virtual void resizeStorage(std::size_t newSize) override;



		std::vector<RadeonRays::float3> data;
		std::vector<float> depthData;

	};









	using RenderOutputPtr = std::shared_ptr<RenderOutput>;


}









#endif

