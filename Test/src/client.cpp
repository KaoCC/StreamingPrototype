
#include <iostream>
#include <boost/asio.hpp>

#include "message.pb.h"
#include "Packet.hpp"
#include "Common.hpp"

using boost::asio::ip::tcp;


SP::CameraConfig globalCamera(SP::Position(0, 0, 0), SP::Direction(0, 0, 0));


int main(int argc, char* argv[]) {


	try {


		boost::asio::io_service ios;
		tcp::resolver res(ios);

		const std::string ip = "127.0.0.1";
		const std::string port = "8051";

		tcp::resolver::query quer(ip, port);
		tcp::resolver::iterator endpoint_iterator = res.resolve(quer);



		tcp::socket streamingSocket(ios);
		boost::asio::connect(streamingSocket, endpoint_iterator);

		// handle data below
		//while (true) {

		//}

		boost::system::error_code error;


		// write Init Message -----------

		SP::Packet::MessagePointer requestPtr(new StreamingFormat::StreamingMessage);

		// allocate on the heap
		StreamingFormat::Init* init = new StreamingFormat::Init;

		// set arguments
		init->set_width(1024);
		init->set_height(768);
		init->set_moduleid(10);

		requestPtr->set_type(StreamingFormat::MessageType::MsgInit);
		requestPtr->set_allocated_initmsg(init);


		SP::Packet::DataBuffer writeBuffer;
		SP::Packet requestPacket(requestPtr);

		if (requestPacket.packing(writeBuffer)) {
			boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));
		} else {
			// Error here
			// Throw
		}


		// Read Response Messages ----------


		// allocate on the heap
		//StreamingFormat::DefaultPos* defPos = new StreamingFormat::DefaultPos;

		SP::Packet::DataBuffer readBuffer;

		// header first
		readBuffer.resize(SP::Packet::HEADER_SIZE);
		boost::asio::read(streamingSocket, boost::asio::buffer(readBuffer));

		// decode header
		SP::Packet::MessagePointer responsePtr(new StreamingFormat::StreamingMessage);
		SP::Packet responsePacket(responsePtr);
		size_t msgSize = responsePacket.decodeHeader(readBuffer);

		// handle the actual msg
		readBuffer.resize(SP::Packet::HEADER_SIZE + msgSize);
		boost::asio::read(streamingSocket, boost::asio::buffer(&readBuffer[SP::Packet::HEADER_SIZE], msgSize));


		SP::Packet::MessagePointer msg = nullptr;
		if (responsePacket.unpacking(readBuffer)) {
			msg = responsePacket.getMessagePtr();
		} else {
			// Error Here !
			// throw ?
		}


		// handle the packet based on the type
		switch (msg->type()) {

		case StreamingFormat::MessageType::MsgDefaultPos:
		{
			globalCamera.pos.x = msg->defaultposmsg().x();
			globalCamera.pos.y = msg->defaultposmsg().y();
			globalCamera.pos.z = msg->defaultposmsg().z();

			globalCamera.dir.vx = msg->defaultposmsg().vx();
			globalCamera.dir.vx = msg->defaultposmsg().vy();
			globalCamera.dir.vx = msg->defaultposmsg().vz();

			// for testing only
			float delta_x = 10;
			float delta_y = -100;
			float delta_z = 1;

			float delta_vx = 5;
			float delta_vy = 10;
			float delta_vz = -20;


			// set the camera delta
			StreamingFormat::Camera* cameraDelta = new StreamingFormat::Camera;
			

			// write camera info (delta) to the server
			requestPtr->Clear();
			requestPtr->set_type(StreamingFormat::MessageType::MsgCameraInfo);
			requestPtr->set_allocated_cameramsg(cameraDelta);

			requestPacket.setMessagePtr(requestPtr);


			if (requestPacket.packing(writeBuffer)) {
				boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));
			} else {
				// Error here
				// Throw
			}

			break;
		}

		case StreamingFormat::MessageType::MsgImage:
		{
			break;
		}


		case StreamingFormat::MessageType::MsgEnding:


			break;


		default:
			// Error !
			break;
		}


		// ...


	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}


	return 0;
}