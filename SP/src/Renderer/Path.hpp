#ifndef _SP_PATH_HPP_
#define _SP_PATH_HPP_

#include "math/float3.h"

namespace SP {

	class Path {

	public:

		bool isAlive();
		bool isScattered();
		bool isSpecular();

		// Scatter Flag
		void setScatterFlag();
		void clearScatterFlag();

		// Specular Flag
		void clearSpecularFlag();
		void setSpecularFlag();

		void restart();

		int getVolumeIdx();
		int getThroughput();
		void multiplyThroughput(RadeonRays::float3 mul);
		void kill();


		// Add contribution ??

	private:

		RadeonRays::float4 throughput;
		int volume;
		int flags;
		int extra_0;
		int extra_1;

	};

}


#endif
