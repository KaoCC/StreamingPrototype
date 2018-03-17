#include "RenderOutput.hpp"

namespace SP {
	RenderOutput::RenderOutput(std::size_t w, std::size_t h) : Output(w, h), data(w * h), depthData(w * h), intersectionData(w  * h) {


	}

	//void RenderOutput::getData(RadeonRays::float3 * dt) const {

	//	//KAOCC: copy or pass pointer ?

	//	// tmp
	//	std::copy(data.begin(), data.end(), dt);
	//	
	//	//throw std::runtime_error("Yet to be done");
	//}

	std::vector<RadeonRays::float3> RenderOutput::copyData() const {
		return data;
	}

	std::vector<float> RenderOutput::copyDepthData() const {
		return depthData;
	}


	RadeonRays::float3& RenderOutput::operator[](std::size_t pos) {
		return data[pos];
	}

	const RadeonRays::float3& RenderOutput::operator[](std::size_t pos) const {
		return data[pos];
	}

	float& RenderOutput::getDepthValue(std::size_t pos) {
		return depthData[pos];
	}

	const float& RenderOutput::getDepthValue(std::size_t pos) const {
		return depthData[pos];
	}

	std::size_t RenderOutput::getStorageSize() const {
		return data.size();
	}

	void RenderOutput::resizeStorage(std::size_t newSize) {
		data.resize(newSize);
		depthData.resize(newSize);
		intersectionData.resize(newSize);
	}

	void RenderOutput::resetStorageDefault() {
		std::fill(data.begin(), data.end(), 0.0f);
		std::fill(depthData.begin(), depthData.end(), 0.0f);
		std::fill(intersectionData.begin(), intersectionData.end(), RadeonRays::Intersection());
	}

	std::vector<RadeonRays::float3>& RenderOutput::getData()
	{
		return data;
	}

	std::vector<float>& RenderOutput::getDepthData() {
		return depthData;
	}

	std::vector<RadeonRays::Intersection>& RenderOutput::getIntersectionData()
	{
		return intersectionData;
	}

}
