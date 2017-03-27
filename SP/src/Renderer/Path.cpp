#include "Path.hpp"



namespace SP {





	bool Path::isAlive() {
		return (flags & kKilled) == 0;
	}

	bool Path::isScattered() {
		return flags & kScattered;
	}

	bool Path::isSpecular() {
		return flags & kSpecularBounce;
	}

	void Path::setScatterFlag() {
		flags |= kScattered;
	}

	void Path::clearScatterFlag() {
		flags &= ~kScattered;
	}

	void Path::setSpecularFlag() {
		flags |= kSpecularBounce;
	}

	void Path::clearSpecularFlag() {
		flags &= ~kSpecularBounce;
	}

	void Path::restart() {
		flags = 0;
	}

	int Path::getVolumeIdx() {
		return volume;
	}

	RadeonRays::float3 Path::getThroughput() {
		return throughput;
	}

	void Path::multiplyThroughput(RadeonRays::float3 mul) {
		throughput *= mul;
	}

	void Path::kill() {
		flags |= kKilled;
	}

}




