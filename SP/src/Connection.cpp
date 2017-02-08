#include "Connection.hpp"


namespace SP {

	Connection::ConnectionPointer Connection::create(boost::asio::io_service & ios) {
		return ConnectionPointer(new Connection(ios));
	}

	Connection::Connection(boost::asio::io_service & ios) : 
		streamingSocket(ios), packet(Packet::MessagePointer(new StreamingFormat::StreamingMessage())) {
	}

	void Connection::start() {
		startReadHeader();
	}

	void Connection::startReadHeader() {
		localBuffer.resize(Packet::HEADER_SIZE);

		boost::asio::async_read(streamingSocket, boost::asio::buffer(localBuffer), 
			std::bind(&Connection::handleReadHeader, shared_from_this(), std::placeholders::_1));


	}

	void Connection::handleReadHeader(const boost::system::error_code & error) {

		if (!error) {
			size_t msgSize = packet.decodeHeader(localBuffer);

			startReadMessage(msgSize);
		}

	}

	void Connection::startReadMessage(size_t msgSize) {

		localBuffer.resize(Packet::HEADER_SIZE + msgSize);
		boost::asio::mutable_buffers_1 mutableBuffer = boost::asio::buffer(&localBuffer[Packet::HEADER_SIZE], msgSize);

		boost::asio::async_read(streamingSocket, mutableBuffer, 
			std::bind(&Connection::handleReadMessage, shared_from_this(), std::placeholders::_1));

	}

	void Connection::handleReadMessage(const boost::system::error_code & error) {

		if (!error) {

			// KAOCC: TODO: Yet to be done !!!!!!!

			resolvePacket();


			// more here !!!
		}


	}

	void Connection::resolvePacket() {

		if (packet.unpacking(localBuffer)) {
			Packet::MessagePointer msgPtr = packet.getMessagePtr();


			Packet::MessagePointer responsePtr = createResponse(msgPtr);
			// More here ...

		}

	}

	Packet::MessagePointer Connection::createResponse(Packet::MessagePointer msgPtr) {

		// KAOCC: Yet to be done !


		switch (msgPtr->type()) {
		}


		// KAOCC: check the return !
		return Packet::MessagePointer();
	}


	boost::asio::ip::tcp::socket & Connection::getSocketRef() {
		return streamingSocket;
	}






}
