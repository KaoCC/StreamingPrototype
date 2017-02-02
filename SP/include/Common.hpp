
#ifndef _SP_COMMON_HPP_
#define _SP_COMMON_HPP_


#include <cstdint>


namespace SP {



	enum PacketType : uint8_t {
		PkktInit = 0,
		PktDefaultPos = 1,
		PktCameraInfo = 2,
		PktImage = 3,
		PktEnding = 4
	};


	// Host Format !
	// Pending: ntohs, nothl, htons, and htonl.

	struct InItPacket {
		const uint8_t type = PkktInit;
		uint32_t width;
		uint32_t hight;
		uint32_t moduleID;

		// Time ?
	};

	struct DefaultPosPacket {
		const uint8_t type = PktDefaultPos;
		uint32_t x;
		uint32_t y;
		uint32_t z;
		uint32_t vx;
		uint32_t vy;
		uint32_t vz;

	};

	struct CameraPacket {
		const uint8_t type = PktCameraInfo;
		uint32_t serialNumber;
		uint32_t delta_x;
		uint32_t delta_y;
		uint32_t delta_z;
		uint32_t delta_vx;
		uint32_t delta_vy;
		uint32_t delta_vz;
	};


	struct ImagePacket {
		const uint8_t type = PktImage;
		uint32_t serialNumber;
		uint32_t status;
		// time ?
		uint32_t size;
		// data ?
		// ...
	};


	struct EndingPacket {
		const uint8_t type = PktEnding;
	};

	// KAOCC:
	// need a lot of memcpy stuff ....

}






#endif