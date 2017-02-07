#include "Packet.hpp"


namespace SP {

	Packet::Packet(MessagePointer msgPtr) : messagePtr(msgPtr) {
	}

	void Packet::setMessagePtr(MessagePointer msgPtr) {
		messagePtr = msgPtr;
	}

	Packet::MessagePointer Packet::getMessagePtr() {
		return messagePtr;
	}

	bool Packet::packing(DataBuffer & buffer) const {

		// check if the pointer is valid
		if (!messagePtr) {
			return false;
		}

		size_t msgSize = messagePtr->ByteSize();
		buffer.resize(HEADER_SIZE + msgSize);

		encodeHeader(buffer, msgSize);

		// KAOCC: check the argument
		return messagePtr->SerializeWithCachedSizesToArray(&buffer[HEADER_SIZE]);

	}

	bool Packet::unpacking(const DataBuffer & buffer) {
		//return messagePtr->;
	}

	size_t Packet::decodeHeader(const DataBuffer & buffer) const {

		if (buffer.size() < HEADER_SIZE) {
			// KAOCC: throw ?
			// Error here
			return 0;
		}

		size_t msgSize = 0;
		for (size_t i = 0; i < HEADER_SIZE; ++i) {
			// KAOCC: check the endianness
			msgSize = msgSize * 256 + static_cast<size_t>(buffer[i] & 0xFF);
		}

		return msgSize;
	}

	void Packet::encodeHeader(DataBuffer & buffer, size_t size) const {

		// KAOCC: check the endianness

		buffer[0] = static_cast<uint8_t>((size >> 24) & 0xFF);
		buffer[1] = static_cast<uint8_t>((size >> 16) & 0xFF);
		buffer[2] = static_cast<uint8_t>((size >> 8) & 0xFF);
		buffer[3] = static_cast<uint8_t>((size) & 0xFF);
	}









}
