#ifndef _SP_PATH_HPP_
#define _SP_PATH_HPP_

#include "math/float3.h"

namespace SP {

	class Path {

	public:


		enum PathFlags {
			kNone = 0x0,
			kKilled = 0x1,
			kScattered = 0x2,
			kSpecularBounce = 0x4
		};


		void initGen();

		bool isAlive();
		bool isScattered();
		bool isSpecular();

		// Scatter Flag
		void setScatterFlag();
		void clearScatterFlag();

		// Specular Flag
		void setSpecularFlag();
		void clearSpecularFlag();

		void restart();

		int getVolumeIdx();
		RadeonRays::float3 getThroughput();
		void multiplyThroughput(RadeonRays::float3 mul);

		void kill();


		// Add contribution ??

	private:

		RadeonRays::float4 throughput;
		int volume;
		int flags;
		int active;
		int extra_1;

	};

}


#endif
