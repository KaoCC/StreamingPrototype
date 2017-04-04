
#include <iostream>
#include <boost/asio.hpp>

#include <vector>

#include "message.pb.h"
#include "Packet.hpp"
#include "Common.hpp"

using boost::asio::ip::tcp;


SP::CameraConfig globalCamera(SP::Position(0, 0, 0), SP::Direction(0, 0, 0));

std::vector<SP::ImageConfig> images;
const unsigned MAX_IMAGE_COUNT = 20;


// for testing only
const float delta_x = 10;
const float delta_y = -100;
const float delta_z = 1;

const float delta_vx = 5;
const float delta_vy = 10;
const float delta_vz = -20;

static float shift = 0;

// simple helper
void configCameraDelta(uint32_t serialNumber, SP::Packet::MessagePointer& requestPtr, SP::Packet& requestPacket) {


	globalCamera.pos.x += delta_x + shift;
	globalCamera.pos.y += delta_y;
	globalCamera.pos.z += delta_z;

	globalCamera.dir.vx += delta_vx;
	globalCamera.dir.vy += delta_vy;
	globalCamera.dir.vz += delta_vz;

	// set the camera delta
	StreamingFormat::Camera* cameraDelta = new StreamingFormat::Camera;

	cameraDelta->set_serialnumber(serialNumber);

	cameraDelta->set_delta_x(globalCamera.pos.x);
	cameraDelta->set_delta_y(delta_y);
	cameraDelta->set_delta_z(delta_z);

	cameraDelta->set_delta_vx(delta_x);
	cameraDelta->set_delta_vy(delta_y);
	cameraDelta->set_delta_vz(delta_z);

	// test
	shift += 0.1f;

	// write camera info (delta) to the server
	requestPtr->Clear();
	requestPtr->set_type(StreamingFormat::MessageType::MsgCameraInfo);
	requestPtr->set_allocated_cameramsg(cameraDelta);

	requestPacket.setMessagePtr(requestPtr);
}


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

		std::cerr << "Set Init" << std::endl;

		requestPtr->set_type(StreamingFormat::MessageType::MsgInit);
		requestPtr->set_allocated_initmsg(init);


		SP::Packet::DataBuffer writeBuffer;
		SP::Packet requestPacket(requestPtr);

		std::cerr << "Pack" << std::endl;

		if (requestPacket.packing(writeBuffer)) {
			boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));
		} else {

			std::cerr << "MsgInit: packing failed !" << std::endl;
			// Error here
			// Throw
		}


		std::cerr << "Active" << std::endl;

		// Read Response Messages ----------


		bool activeFlag = true;
		uint32_t serialNumber = 0;

		// currently not an optimal setting ...
		while (activeFlag) {

			SP::Packet::DataBuffer readBuffer;

			// header first
			readBuffer.resize(SP::Packet::HEADER_SIZE);
			boost::asio::read(streamingSocket, boost::asio::buffer(readBuffer));

			std::cerr << "After Read" << std::endl;

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

				std::cerr << "Null message !" << std::endl;
				// Error Here !
				// throw ?
			}

			// test only !!!!!
			//uint32_t serialNumber = 0;

			images.resize(MAX_IMAGE_COUNT);

			// handle the packet based on the type
			switch (msg->type()) {

			case StreamingFormat::MessageType::MsgDefaultPos:
			{

				std::cerr << "MsgDefaultPos !" << std::endl;

				globalCamera.pos.x = msg->defaultposmsg().x();
				globalCamera.pos.y = msg->defaultposmsg().y();
				globalCamera.pos.z = msg->defaultposmsg().z();

				std::cerr << "x, y, z: " << globalCamera.pos.x << " " <<globalCamera.pos.y << " " <<globalCamera.pos.z << std::endl;

				globalCamera.dir.vx = msg->defaultposmsg().vx();
				globalCamera.dir.vy = msg->defaultposmsg().vy();
				globalCamera.dir.vz = msg->defaultposmsg().vz();

				std::cerr << "vx, vy, vz: " << globalCamera.dir.vx << " " << globalCamera.dir.vy << " " << globalCamera.dir.vz << std::endl;

				configCameraDelta(serialNumber, requestPtr, requestPacket);

				break;
			}

			case StreamingFormat::MessageType::MsgImage:
			{
				std::cerr << "MsgImage !" << std::endl;

				uint32_t returnedSerialNumber = msg->imagemsg().serialnumber();
				std::cerr << "returned Serial number: " << returnedSerialNumber << std::endl;

				// should be "8051"
				uint32_t statusCode = msg->imagemsg().status();
				std::cerr << "status code: >>> " << statusCode << std::endl;

				// resolve byte size
				uint32_t byteSize = msg->imagemsg().bytesize();
				std::cerr << "byteSize:" << byteSize << std::endl;

				// where is the image ?
				// get the image

				// this one should be avoid
				images[returnedSerialNumber].getImageData().resize(byteSize);

				boost::asio::read(streamingSocket, boost::asio::buffer(images[returnedSerialNumber].getImageData(), byteSize));

				//std::copy(msg->imagemsg().imagedata().begin(), msg->imagemsg().imagedata().end(), images[returnedSerialNumber].getImageData().begin());

				std::cerr << "Get image !" << std::endl;

				// visualize some of them :)

				std::cerr << "the first 10 elements: " << std::endl;
				for (int v = 0; v < 10; ++v) {
					std::cerr << (int) images[returnedSerialNumber].getImageData()[v] <<" ";
				}
				std::cerr << std::endl;

				// generate next camera delta
				++serialNumber;
				configCameraDelta(serialNumber, requestPtr, requestPacket);


				break;
			}


			case StreamingFormat::MessageType::MsgEnding:
			{
				// Yet to be done !


				break;
			}


			default:
				// Error !
				break;
			}

			// test only
			// overwrite the msg with ENDING message
			if (serialNumber >= MAX_IMAGE_COUNT) {

				StreamingFormat::Ending* end = new StreamingFormat::Ending;

				requestPtr->Clear();
				requestPtr->set_type(StreamingFormat::MessageType::MsgEnding);
				requestPtr->set_allocated_endingmsg(end);

				activeFlag = false;
			}

			// write the packet out
			if (requestPacket.packing(writeBuffer)) {
				boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));
			} else {
				// Error here
				// Throw
				std::cerr << "Write: packing failed !" << std::endl;
			}




		}

		// end of the connection.

		std::cerr << "End of the connection !!!" << std::endl;

		// ...


	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}


	return 0;
}