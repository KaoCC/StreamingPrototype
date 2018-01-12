
#include "SimpleRenderer.hpp"
#include "Scene/Camera.hpp"

#include "BxDFHelper.hpp"

namespace SP {


	// tmp , workaround
	RadeonRays::float3 tmpGetValue(const Material* matPtr) {
		auto single = dynamic_cast<const SingleBxDF*>(matPtr);

		if (single) {
			return single->getInputValue("albedo").floatValue;
		} else {
			// tmp
			return RadeonRays::float3 {0.3f, 0.3f, 0.3f};
		}

	}

	SimpleRenderer::SimpleRenderer(ApiEngine& engine) : mEngineRef { engine } {
	}




	void SimpleRenderer::preprocess(Scene const& scene) {
	}

	// OBJ viewer
	void SimpleRenderer::render(Scene const& scene, size_t configIdx) {


		generatePrimaryRays(scene, configIdx);

		int maxrays = mRenderOutPtr->getWidth() * mRenderOutPtr->getHeight();

		mSimpleRenderDataPtr.host_hitcount = maxrays;


		mEngineRef.queryIntersection(mSimpleRenderDataPtr.host_rays[0], mSimpleRenderDataPtr.host_hitcount, mSimpleRenderDataPtr.host_intersections).wait();

		simpleShading(scene);
	}

	void SimpleRenderer::setOutput(std::shared_ptr<RenderOutput> output) {

		if (!mRenderOutPtr || mRenderOutPtr->getWidth() < output->getWidth() || mRenderOutPtr->getHeight() < output->getHeight()) {
			resizeWorkingSet(*output);
		}

		mRenderOutPtr = output;
	}

	SimpleRenderer::~SimpleRenderer() = default;

	void SimpleRenderer::generatePrimaryRays(const Scene& scene, size_t camIdx) {

		const std::size_t imageWidth = mRenderOutPtr->getWidth();
		const std::size_t imageHeight = mRenderOutPtr->getHeight();

		const std::uint32_t rngseed = RadeonRays::rand_uint();


		for (std::uint32_t y = 0; y < imageHeight; ++y) {
			for (std::uint32_t x = 0; x < imageWidth; ++x) {

				const PerspectiveCamera& cameraRef { static_cast<const PerspectiveCamera&>(scene.getCamera(camIdx)) };
				RadeonRays::ray& currentRay = mSimpleRenderDataPtr.host_rays[0][y * imageWidth + x];
				generateRandomRay(rngseed, x, y, imageWidth, imageHeight, currentRay, cameraRef);

			}
		}


	}

	void SimpleRenderer::resizeWorkingSet(const Output& out) {

		mSimpleRenderDataPtr.host_rays[0].clear();
		mSimpleRenderDataPtr.host_rays[0].resize(out.getWidth() * out.getHeight());

		mSimpleRenderDataPtr.host_intersections.clear();
		mSimpleRenderDataPtr.host_intersections.resize(out.getWidth() * out.getHeight());

		mSimpleRenderDataPtr.host_hitcount = 0;

	}

	void SimpleRenderer::simpleShading(const Scene& scene) {

		const std::vector<RadeonRays::ray>& rayArrayRef = mSimpleRenderDataPtr.host_rays[0];

		auto& outRef = *mRenderOutPtr;

		//const std::vector<const Mesh*>& meshPtrs = mEngineRef.getInternalMeshPtrs();


		for (auto i = 0; i < mSimpleRenderDataPtr.host_hitcount; ++i) {

			const RadeonRays::Intersection& isectRef = mSimpleRenderDataPtr.host_intersections[i];

			if (isectRef.shapeid == -1) {

				// color black !
				outRef[i] = RadeonRays::float3(0, 0, 0);
			} else {


				int shapeId = isectRef.shapeid - 1;
				const Mesh& meshData = dynamic_cast<const Mesh&>(scene.getShape(shapeId));

				const Material* matPtr = meshData.getMaterial();


				const uint32_t* indexArray = meshData.getIndices();

				int primId = isectRef.primid;

				const uint32_t& i0 = indexArray[3 * primId];                                // CHECK ?
				const uint32_t& i1 = indexArray[3 * primId + 1];
				const uint32_t& i2 = indexArray[3 * primId + 2];

				const RadeonRays::float3* normalArray = meshData.getNormals();

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

				RadeonRays::float2 localUV;                        // check this !
				localUV.x = isectRef.uvwt.x;
				localUV.y = isectRef.uvwt.y;

				auto normal = RadeonRays::normalize(
						RadeonRays::transform_vector((1.f - localUV.x - localUV.y) * n0 + localUV.x * n1 + localUV.y * n2, matrixI));  //CHECK THIS !

				if (RadeonRays::dot(normal, -rayArrayRef[i].d) < 0) {
					normal = -normal;
				}


				// FIXME: bug here for multi-bxdf
				outRef[i] = tmpGetValue(matPtr) * RadeonRays::dot(-rayArrayRef[i].d, normal);        // check ray direction

			}

			outRef[i].w += 1;

		}

	}


}


