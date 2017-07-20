
#include "SimpleRenderer.hpp"
#include "Scene/Camera.hpp"

#include "BxDFHelper.hpp"

namespace SP {


	struct SimpleRenderer::SimpleRenderData{
		std::vector<RadeonRays::ray> host_rays[2];

		std::vector<RadeonRays::Intersection> host_intersections;
		int host_hitcount;
	};

	SimpleRenderer::SimpleRenderer(ApiEngine& engine) : mSimpleRenderDataPtr{ new SimpleRenderData }, mEngineRef{ engine } {
	}


	std::shared_ptr<Output>  SimpleRenderer::createOutput(std::uint32_t w, std::uint32_t h) const {
		return std::shared_ptr<RenderOutput>(new RenderOutput(w, h));
	}

	//void SimpleRenderer::deleteOutput(Output * output) const {
	//	delete output;
	//}


	// KAOCC: tmp ! not an accurate approach
	void SimpleRenderer::clear(RadeonRays::float3 const & val, Output & output) const {
		RenderOutput& rendOutRef = dynamic_cast<RenderOutput&>(output); 		// test it !

		auto& storedData = rendOutRef.getInternalStorage();

		for (auto& data : storedData) {
			data = val;
		}
	}

	void SimpleRenderer::preprocess(Scene const & scene) {
	}

	// OBJ viewer
	void SimpleRenderer::render(Scene const & scene, size_t configIdx) {


		generatePrimaryRays(scene, configIdx);

		int maxrays = mRenderOutPtr->getWidth() * mRenderOutPtr->getHeight();

		mSimpleRenderDataPtr->host_hitcount = maxrays;


		mEngineRef.queryIntersection(mSimpleRenderDataPtr->host_rays[0], mSimpleRenderDataPtr->host_hitcount, mSimpleRenderDataPtr->host_intersections).wait();

		simpleShading();
	}

	void SimpleRenderer::setOutput(std::shared_ptr<Output> output) {

		if (!mRenderOutPtr || mRenderOutPtr->getWidth() < output->getWidth() || mRenderOutPtr->getHeight() < output->getHeight()) {
			resizeWorkingSet(*output);
		}

		mRenderOutPtr = std::dynamic_pointer_cast<RenderOutput>(output);
	}

	SimpleRenderer::~SimpleRenderer() = default;

	void SimpleRenderer::generatePrimaryRays(const Scene & scene, size_t camIdx) {

		const uint32_t imageWidth = mRenderOutPtr->getWidth();
		const uint32_t imageHeight = mRenderOutPtr->getHeight();

		uint32_t rngseed = RadeonRays::rand_uint();


		for (uint32_t y = 0; y < imageHeight; ++y) {
			for (uint32_t x = 0; x < imageWidth; ++x) {

				const PerspectiveCamera* cameraPtr{ static_cast<const PerspectiveCamera*>(scene.getCamera(camIdx)) };
				RadeonRays::ray& currentRay = mSimpleRenderDataPtr->host_rays[0][y * imageWidth + x];
				generateRandomRay(rngseed, x, y, imageWidth, imageHeight, currentRay,cameraPtr);

			}
		}


	}

	void SimpleRenderer::resizeWorkingSet(const Output & out) {

		mSimpleRenderDataPtr->host_rays[0].clear();
		mSimpleRenderDataPtr->host_rays[0].resize(out.getWidth() * out.getHeight());

		mSimpleRenderDataPtr->host_intersections.clear();
		mSimpleRenderDataPtr->host_intersections.resize(out.getWidth() * out.getHeight());

		mSimpleRenderDataPtr->host_hitcount = 0;

	}

	void SimpleRenderer::simpleShading() {

		const std::vector<RadeonRays::ray>& rayArrayRef = mSimpleRenderDataPtr->host_rays[0];

		std::vector<RadeonRays::float3>& outRef = mRenderOutPtr->getInternalStorage();

		const std::vector<const Mesh*>& meshPtrs = mEngineRef.getInternalMeshPtrs();

		

		for (size_t i = 0; i < mSimpleRenderDataPtr->host_hitcount; ++i) {

			const RadeonRays::Intersection& isectRef = mSimpleRenderDataPtr->host_intersections[i];

			if (isectRef.shapeid == -1) {

				// color black !
				outRef[i] = RadeonRays::float3(0, 0, 0);
			} else {


				int shapeId = isectRef.shapeid - 1;
				const Mesh* meshDataPtr = meshPtrs[shapeId];

				const Material* matPtr = meshDataPtr->getMaterial();


				const uint32_t* indexArray = meshDataPtr->getIndices();

				int primId = isectRef.primid;

				const uint32_t& i0 = indexArray[3 * primId];								// CHECK ?
				const uint32_t& i1 = indexArray[3 * primId + 1];
				const uint32_t& i2 = indexArray[3 * primId + 2];

				const RadeonRays::float3* normalArray = meshDataPtr->getNormals();

				const RadeonRays::float3& n0 = normalArray[i0];
				const RadeonRays::float3& n1 = normalArray[i1];
				const RadeonRays::float3& n2 = normalArray[i2];

				//const RadeonRays::float3* verticeArray = meshDataPtr->getVertices();

				//const RadeonRays::float3& v0 = verticeArray[i0];
				//const RadeonRays::float3& v1 = verticeArray[i1];
				//const RadeonRays::float3& v2 = verticeArray[i2];

				//const RadeonRays::float2* uvArray = meshDataPtr->getUVs();

				//const RadeonRays::float2& uv0 = uvArray[i0];
				//const RadeonRays::float2& uv1 = uvArray[i1];
				//const RadeonRays::float2& uv2 = uvArray[i2];

				//auto normalGeo = RadeonRays::normalize(RadeonRays::cross(v1 - v0, v2 - v0));

				RadeonRays::float2 localUV;						// check this !
				localUV.x = isectRef.uvwt.x;
				localUV.y = isectRef.uvwt.y;

				auto normal = RadeonRays::normalize(RadeonRays::transform_vector((1.f - localUV.x - localUV.y) * n0 + localUV.x * n1 + localUV.y * n2, matrixI));  //CHECK THIS !

				if (RadeonRays::dot(normal, -rayArrayRef[i].d) < 0) {
					normal = -normal;
				}

				outRef[i] = matPtr->getInputValue("albedo").floatValue * RadeonRays::dot(-rayArrayRef[i].d, normal);		// check ray direction

			}

			outRef[i].w += 1;

		}

	}




}


