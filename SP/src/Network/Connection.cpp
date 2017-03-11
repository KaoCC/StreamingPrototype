#include "Connection.hpp"

#include <iostream>

namespace SP {

	Connection::ConnectionPointer Connection::createWithBuffer(boost::asio::io_service & ios, SyncBuffer<ImageConfig>& buf) {
		return ConnectionPointer(new Connection(ios, buf));
	}

	Connection::Connection(boost::asio::io_service & ios, SyncBuffer<ImageConfig>& buf) :
		streamingSocket(ios), packet(Packet::MessagePointer(new StreamingFormat::StreamingMessage())), cfgManager(buf)
		, responsePacket(Packet::MessagePointer(new StreamingFormat::StreamingMessage())) 
	{
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


	// KAOCC: need to change !!
	void Connection::handleWriteMessage(const boost::system::error_code & error) {

		if (!error) {

			writeBufferQueue.pop_front();


			// if not empty ?

			if (!writeBufferQueue.empty()) {
				boost::asio::async_write(streamingSocket, boost::asio::buffer(writeBufferQueue.front()),
					std::bind(&Connection::handleWriteMessage, shared_from_this(), std::placeholders::_1));
			}


		} else {
			std::cerr << "---------------- Write Error: " << error << '\n';
			boost::system::error_code ignored_ec;
			//streamingSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			//streamingSocket.close();
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
		Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };

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
			StreamingFormat::DefaultPos* defPosPtr{ new StreamingFormat::DefaultPos };

			// for testing only
			CameraConfig camCfg{ cfgManager.getCamera() };

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

			StreamingFormat::Image* imagePtr{ new StreamingFormat::Image };

			// for testing only
			imagePtr->set_serialnumber(serialNumber);
			

			ImageConfig& imageData{ cfgManager.getImage() };
			ImageConfig::ImageBuffer& imageBufferCache{ imageData.getImageData() };

			std::cerr << "[IMG CACHE SZ]: " << imageBufferCache.size() << " >>>>>>>>>>>>  ID: " << imageData.getID() << '\n';


			Encoder* encoder = cfgManager.getEncoder();
			uint8_t* rawPtr = encoder->getEncoderRawBuffer();

			std::copy(imageBufferCache.begin(), imageBufferCache.end(), rawPtr);

			uint8_t* outBufPtr;
			int outSize = 0;
			encoder->startEncoding(&outBufPtr, &outSize);

			//ImageConfig::ImageBuffer encodedImageData;
			if (outSize > 0) {

				std::cerr << "SUCCESS! Size: " << outSize << '\n';

				//ImageBuffer encodedImageData(outBufPtr, outBufPtr + outSize);

				//tmp
				encodedImageData = std::move(ImageConfig::ImageBuffer(outBufPtr, outBufPtr + outSize));

				// accumulate
				//accBuffer.insert(std::end(accBuffer), std::begin(imageData), std::end(imageData));

			} else {
				std::cerr << "Failed to Encode !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << '\n';
			}


			imagePtr->set_bytesize(encodedImageData.size()); // tmp
			imagePtr->set_status(imageData.getID());  // tmp


			// image data ????
			// need to check
			//imagePtr->set_imagedata(reinterpret_cast<const char*>(imageData.getImageRawData()));

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
		//Packet responsePacket(msgPtr);

		//writeBuffer.clear();
		responsePacket.setMessagePtr(msgPtr);

		if (responsePacket.packing(writeBuffer)) {


			// check if we need to write the image
			if (msgPtr->type() == StreamingFormat::MessageType::MsgImage) {

				//boost::asio::write(streamingSocket, boost::asio::buffer(cfgManager.getImageCache().getImageData()));
				/*boost::asio::async_write(streamingSocket, boost::asio::buffer(encodedImageData),
					std::bind(&Connection::handleWriteImage, shared_from_this(), std::placeholders::_1)); */
				//boost::asio::write(streamingSocket, boost::asio::buffer(encodedImageData));

				appendImage(writeBuffer);

			}


			bool writeInProgress = !writeBufferQueue.empty();  // check for image queue also ?
			writeBufferQueue.push_back(std::move(writeBuffer));

			if (!writeInProgress) {


				/*boost::asio::async_write(streamingSocket, boost::asio::buffer(writeBuffer),
					std::bind(&Connection::handleWriteMessage, shared_from_this(), std::placeholders::_1)); */
				//boost::asio::write(streamingSocket, boost::asio::buffer(writeBuffer));

				boost::asio::async_write(streamingSocket, boost::asio::buffer(writeBufferQueue.front()),
					std::bind(&Connection::handleWriteMessage, shared_from_this(), std::placeholders::_1));

			}



		} else {
			// Error here
			// Throw ???

			std::cerr << "Failed to pack !!! " << std::endl;
		}

	}


	// yet to be done
	void Connection::appendImage(Packet::DataBuffer & buffer) {

		//size_t currentSize = buffer.size();
		//size_t newSize = currentSize + encodedImageData.size();
		//buffer.resize(newSize);
		// test
		//std::cerr << "Resize " << buffer.size() << '\n';
		//std::copy(encodedImageData.begin(), encodedImageData.end(), buffer.end());


		buffer.insert(std::end(buffer), std::begin(encodedImageData), std::end(encodedImageData));

		// test
		//std::cerr << "insert size " << buffer.size() << '\n';
	}


	boost::asio::ip::tcp::socket & Connection::getSocketRef() {
		return streamingSocket;
	}






}
