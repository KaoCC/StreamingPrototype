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

		// check the lifetime ?
		//boost::asio::mutable_buffers_1 mutableBuffer = boost::asio::buffer(&localBuffer[Packet::HEADER_SIZE], msgSize);

		boost::asio::async_read(streamingSocket, boost::asio::buffer(&localBuffer[Packet::HEADER_SIZE], msgSize),
			std::bind(&Connection::handleReadMessage, shared_from_this(), std::placeholders::_1));

	}

	void Connection::handleReadMessage(const boost::system::error_code & error) {

		if (!error) {

			// KAOCC: TODO: Yet to be done !!!!!!!

			// KAOCC: throw exception here if null ?
			Packet::MessagePointer msgPtr = resolvePacket();

			writeResponse(createResponse(msgPtr));

			// repeat
			startReadHeader();
		}


	}

	Packet::MessagePointer Connection::resolvePacket() {

		if (packet.unpacking(localBuffer)) {
			return packet.getMessagePtr();
		} else {
			return nullptr;
		}

	}

	Packet::MessagePointer Connection::createResponse(Packet::MessagePointer msgPtr) {

		// KAOCC: Yet to be done !

		// KAOCC: TODO: prevent creating new Messages for optimization
		Packet::MessagePointer responsePtr(new StreamingFormat::StreamingMessage);

		switch (msgPtr->type()) {
		case StreamingFormat::MessageType::MsgInit:
		{

			// read the msg (init info)

			// KAOCC: do we need a lock ?
			// KAOCC: currently we have one cfg per connection
			cfgManager.setScreen(msgPtr->initmsg().width(), msgPtr->initmsg().height());
			cfgManager.setModuleID(msgPtr->initmsg().moduleid());


			// more from here
			//...


			// reply the default positions
			responsePtr->set_type(StreamingFormat::MessageType::MsgDefaultPos);

			// must be on the heap
			StreamingFormat::DefaultPos* defPosPtr = new StreamingFormat::DefaultPos;

			// for testing only
			CameraConfig camCfg = cfgManager.getCamera();

			defPosPtr->set_x(camCfg.pos.x);
			defPosPtr->set_y(camCfg.pos.y);
			defPosPtr->set_z(camCfg.pos.z);

			defPosPtr->set_vx(camCfg.dir.vx);
			defPosPtr->set_vy(camCfg.dir.vy);
			defPosPtr->set_vz(camCfg.dir.vz);

			responsePtr->set_allocated_defaultposmsg(defPosPtr);

			break;

		}

		case StreamingFormat::MessageType::MsgDefaultPos:
			// Possible Error
			// The server will not get this one!
			break;

		case StreamingFormat::MessageType::MsgCameraInfo:
		{

			// read the msg (camera info)

			uint32_t serialNumber = msgPtr->cameramsg().serialnumber();

			// more here
			//...
			// should read camera info

			float dx = msgPtr->cameramsg().delta_x();
			float dy = msgPtr->cameramsg().delta_y();
			float dz = msgPtr->cameramsg().delta_z();
			
			float dvx = msgPtr->cameramsg().delta_vx();
			float dvy = msgPtr->cameramsg().delta_vy();
			float dvz = msgPtr->cameramsg().delta_vz();

			// KAOCC: check if we need locks 

			// position
			cfgManager.setPositionDelta(dx, dy, dz);

			// direction ?
			// missing ...


			// insert rendering code & encoder here


			// reply the images
			responsePtr->set_type(StreamingFormat::MessageType::MsgImage);

			StreamingFormat::Image* imagePtr = new StreamingFormat::Image;

			// for testing only
			imagePtr->set_serialnumber(serialNumber);
			imagePtr->set_status(8051);  // tmp
			imagePtr->set_bytesize(cfgManager.getImageRef(serialNumber).getByteSize()); // tmp

			// image data ????
			// need to check
			imagePtr->set_imagedata(reinterpret_cast<const char*>(cfgManager.getImageRef(serialNumber).getImageRawData()));

			responsePtr->set_allocated_imagemsg(imagePtr);

			break;
		}

		case StreamingFormat::MessageType::MsgImage:
			// Possible error
			// The server will not get this one !
			break;

		case StreamingFormat::MessageType::MsgEnding:

			// tmp: for testing only
			// Echo the Ending msg back ?
			responsePtr->set_type(StreamingFormat::MessageType::MsgEnding);

			// TODO:
			// Close the connection !

			break;

		default:
			// error ?
			// Throw ?
			break;
		}


		return responsePtr;
	}

	void Connection::writeResponse(Packet::MessagePointer msgPtr) {

		Packet::DataBuffer writeBuffer;
		Packet responsePacket(msgPtr);

		if (responsePacket.packing(writeBuffer)) {
			boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));
		} else {
			// Error here
			// Throw ???
		}

	}


	boost::asio::ip::tcp::socket & Connection::getSocketRef() {
		return streamingSocket;
	}






}
