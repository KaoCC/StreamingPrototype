
#ifndef _SP_PACKET_HPP_
#define _SP_PACKET_HPP_

#include <memory>
#include <vector>
#include <cstdint>

#include "message.pb.h"


namespace SP {



	class Packet {

	public:

		static const size_t HEADER_SIZE = 4;

		using MessagePointer = std::shared_ptr<StreamingFormat::StreamingMessage>;
		using DataBuffer = std::vector<uint8_t>;

		Packet(MessagePointer msgPtr);

		void setMessagePtr(MessagePointer msgPtr);
		MessagePointer getMessagePtr();

		bool packing(DataBuffer& buffer) const;
		bool unpacking(const DataBuffer& buffer);

		size_t decodeHeader(const DataBuffer& buffer) const;
		void encodeHeader(DataBuffer& buffer, size_t size) const;


	private:

		MessagePointer messagePtr;

	};


}









#endif

