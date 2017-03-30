#include "Texture.hpp"

#include "radeon_rays.h"

namespace SP {

	//tmp
	// helper
	static RadeonRays::float2 floorF2(RadeonRays::float2 ff) {
		return (RadeonRays::float2(std::floor(ff.x), std::floor(ff.y)));
	}

	Texture::Texture(const std::uint8_t* ptr, RadeonRays::int2 sz, Format fmt) : data(ptr), size(sz), format(fmt) {
	}

	RadeonRays::float3 Texture::sampleEnvMap(RadeonRays::float3 d) {

		float r;
		float phi;
		float theta;

		// to spherical coords
		cartesian_to_spherical(d, r, phi, theta);

		// Map to [0,1]x[0,1] range and reverse Y axis
		RadeonRays::float2 uv;
		uv.x = phi / (2 * PI);
		uv.y = 1.f - theta / PI;

		return sample2D(uv);
	}

	RadeonRays::float4 Texture::sample2D(RadeonRays::float2 uv) {

		int width = size.x;
		int height = size.y;

		uv -= floorF2(uv);

		uv.y = 1.f - uv.y;

		// Calculate integer coordinates
		unsigned x0 = RadeonRays::clamp((unsigned)std::floor(uv.x * width), 0, width - 1);
		unsigned y0 = RadeonRays::clamp((unsigned)std::floor(uv.y * height), 0, height - 1);

		// Calculate samples for linear filtering
		unsigned  x1 = RadeonRays::clamp(x0 + 1, 0, width - 1);
		unsigned y1 = RadeonRays::clamp(y0 + 1, 0, height - 1);

		// Calculate weights for linear filtering
		float wx = uv.x * width - std::floor(uv.x * width);
		float wy = uv.y * height - std::floor(uv.y * height);

		switch (format) {

		case Format::kRGBA32: {

			RadeonRays::float4 const* mydataf = reinterpret_cast<RadeonRays::float4 const*>(data.get());		// check this conversion

			// Get 4 values for linear filtering
			RadeonRays::float4 val00 = *(mydataf + width * y0 + x0);
			RadeonRays::float4 val01 = *(mydataf + width * y0 + x1);
			RadeonRays::float4 val10 = *(mydataf + width * y1 + x0);
			RadeonRays::float4 val11 = *(mydataf + width * y1 + x1);

			// Filter and return the result
			return RadeonRays::lerp(RadeonRays::lerp(val00, val01, wx), RadeonRays::lerp(val10, val11, wx), wy);
		}

		case Format::kRGBA16: {

			// NOT CHECK YET !!

			struct uhalf4 {
				uint16_t x;
				uint16_t y;
				uint16_t z;
				uint16_t w;
			};

			const uhalf4* mydatah = reinterpret_cast<uhalf4 const*>(data.get());

			// Get 4 values and convert to float
			uhalf4 valu00 = *(mydatah + width * y0 + x0);
			uhalf4 valu01 = *(mydatah + width * y0 + x1);
			uhalf4 valu10 = *(mydatah + width * y1 + x0);
			uhalf4 valu11 = *(mydatah + width * y1 + x1);

			RadeonRays::float4 val00 = RadeonRays::float4((float)valu00.x / 255.f, (float)valu00.y / 255.f, (float)valu00.z / 255.f, (float)valu00.w / 255.f);
			RadeonRays::float4 val01 = RadeonRays::float4((float)valu01.x / 255.f, (float)valu01.y / 255.f, (float)valu01.z / 255.f, (float)valu01.w / 255.f);
			RadeonRays::float4 val10 = RadeonRays::float4((float)valu10.x / 255.f, (float)valu10.y / 255.f, (float)valu10.z / 255.f, (float)valu10.w / 255.f);
			RadeonRays::float4 val11 = RadeonRays::float4((float)valu11.x / 255.f, (float)valu11.y / 255.f, (float)valu11.z / 255.f, (float)valu11.w / 255.f);

			// Filter and return the result
			return RadeonRays::lerp(RadeonRays::lerp(val00, val01, wx), RadeonRays::lerp(val10, val11, wx), wy);


		}


		case Format::kRGBA8: {

			// tmp !!!
			struct uchar4 {
				unsigned char x;
				unsigned char y;
				unsigned char z;
				unsigned char w;
			};

			const uchar4* mydatac = reinterpret_cast<uchar4 const*>(data.get());

			// Get 4 values and convert to float
			uchar4 valu00 = *(mydatac + width * y0 + x0);
			uchar4 valu01 = *(mydatac + width * y0 + x1);
			uchar4 valu10 = *(mydatac + width * y1 + x0);
			uchar4 valu11 = *(mydatac + width * y1 + x1);

			RadeonRays::float4 val00 = RadeonRays::float4((float)valu00.x / 255.f, (float)valu00.y / 255.f, (float)valu00.z / 255.f, (float)valu00.w / 255.f);
			RadeonRays::float4 val01 = RadeonRays::float4((float)valu01.x / 255.f, (float)valu01.y / 255.f, (float)valu01.z / 255.f, (float)valu01.w / 255.f);
			RadeonRays::float4 val10 = RadeonRays::float4((float)valu10.x / 255.f, (float)valu10.y / 255.f, (float)valu10.z / 255.f, (float)valu10.w / 255.f);
			RadeonRays::float4 val11 = RadeonRays::float4((float)valu11.x / 255.f, (float)valu11.y / 255.f, (float)valu11.z / 255.f, (float)valu11.w / 255.f);

			// Filter and return the result
			return RadeonRays::lerp(RadeonRays::lerp(val00, val01, wx), RadeonRays::lerp(val10, val11, wx), wy);

		}

		default:

			return RadeonRays::float4(0, 0, 0, 0);

		}




	}

}
