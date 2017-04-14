#include "Connection.hpp"

#include <iostream>

namespace SP {


	Connection::ConnectionPointer Connection::create(boost::asio::io_service & ios, ConfigManager& configRef) {
		return ConnectionPointer(new Connection(ios, configRef));
	}

	Connection::Connection(boost::asio::io_service & ios, ConfigManager& configRef) :
		streamingSocket(ios), packet(Packet::MessagePointer(new StreamingFormat::StreamingMessage())), mCfgManagerRef(configRef), 
		responsePacket(Packet::MessagePointer(new StreamingFormat::StreamingMessage())) , mEncoder(CreateEncoder(configRef.getScreenWidth(), configRef.getScreenHeight()))
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
			std::cerr << "[ Write Error ] Error Code: " << error << std::endl;
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
//			mCfgManagerRef.setScreen(msgPtr->initmsg().width(), msgPtr->initmsg().height());
			mCfgManagerRef.setModuleID(msgPtr->initmsg().moduleid());


			// more from here
			//...


			// reply the default positions
			responsePtr->set_type(StreamingFormat::MessageType::MsgDefaultPos);

			// must be on the heap
			StreamingFormat::DefaultPos* defPosPtr{ new StreamingFormat::DefaultPos };

			// for testing only
			//CameraConfig camCfg{ mCfgManagerRef.getCamera() };

			// do not change !!!
			defPosPtr->set_x(0);
			defPosPtr->set_y(1);
			defPosPtr->set_z(3);

			defPosPtr->set_vx(0);
			defPosPtr->set_vy(0);
			defPosPtr->set_vz(1);

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


			// TEST !
			if (writeBufferQueue.size() > 3) {
				responsePtr = nullptr;
				break;
			}

			// test
			//if (cachedDeltaX == dx) {
			//	responsePtr = nullptr;
			//	break;
			//} else {
			//	cachedDeltaX = dx;
			//}


			// KAOCC: check if we need locks 

			// position
			//mCfgManagerRef.setPositionDelta(dx, dy, dz);

			// direction ?
			// missing ...

			// insert rendering code & encoder here


			// reply the images
			responsePtr->set_type(StreamingFormat::MessageType::MsgImage);

			StreamingFormat::Image* imagePtr{ new StreamingFormat::Image };

			// for testing only
			imagePtr->set_serialnumber(serialNumber);
			

			//ImageConfig::ImageBuffer& imageBufferCache = cfgManager.getSubLightFieldImages(0);
			//ImageConfig::ImageBuffer imageBufferCache = cfgManager.getAll();

			//std::cerr << "[IMG CACHE SZ]: " << imageBufferCache.size() << " >>>>>>>>>>>>  ID: " << imageData.getID() << '\n';


			uint8_t* rawPtr = mEncoder->getEncoderRawBuffer();

			// TMP !!!
			size_t subLFIndex = mCfgManagerRef.getIndexOfSubLightField(dx);
			size_t subLFSz = mCfgManagerRef.getNumberOfSubLFImages();


			if (!mCfgManagerRef.getSubLightFieldRefreshState(subLFIndex)) {
				std::cerr << "No need to send\n";
				responsePtr = nullptr;
				break;
			} else {
				std::cerr << "Get new: " << subLFIndex << "\n";
			}

			// test
			encodedImageData.clear();

			for (size_t k = 0; k < subLFSz; ++k) {

				// need to optimize for copying !
				//ImageConfig imageData{ cfgManager.getImage() };
				//ImageConfig::ImageBuffer& imageBufferCache{ imageData.getImageData() };

				ImageConfig::ImageBuffer imageBufferCache{ mCfgManagerRef.getSubLightFieldImageWithIndex(subLFIndex, k) };

				std::copy(imageBufferCache.begin(), imageBufferCache.end(), rawPtr);

				uint8_t* outBufPtr;
				int outSize = 0;
				mEncoder->startEncoding(&outBufPtr, &outSize);

				//ImageConfig::ImageBuffer encodedImageData;
				if (outSize > 0) {

					//std::cerr << "SUCCESS! Size: " << outSize << " SubLF Index: " << subLFIndex  << " img Index: " << k << " dx: " << dx << '\n';

					//ImageBuffer encodedImageData(outBufPtr, outBufPtr + outSize);

					//tmp
					//encodedImageData = std::move(ImageConfig::ImageBuffer(outBufPtr, outBufPtr + outSize));

					encodedImageData.insert(std::end(encodedImageData), outBufPtr, outBufPtr + outSize);

					// accumulate
					//accBuffer.insert(std::end(accBuffer), std::begin(imageData), std::end(imageData));

				} else {
					std::cerr << "Failed to Encode !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << '\n';
				}

			}


			// mark read
			//std::cerr << "mark read\n";
			mCfgManagerRef.setSubLightFieldRefreshState(subLFIndex, false);

			//std::cerr << "size" << encodedImageData.size() << std::endl;
			imagePtr->set_bytesize(encodedImageData.size()); // tmp
			//imagePtr->set_status(imageData.getID());  // tmp

			imagePtr->set_status(subLFIndex);	// test

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

		case StreamingFormat::MessageType::MsgControl:
		{
			// test
			std::cerr << "control msg\n";

			const auto& changeFlag = msgPtr->controlmsg().change_scene();
			if (changeFlag) {		// != 0
				// tmp
				std::cerr << "clear Scene\n";
				mCfgManagerRef.clearAll();
			}


			const auto& saveFlag = msgPtr->controlmsg().save_frame();
			if (saveFlag) {  // != 0
				std::cerr << "Save to PPM\n";
				mCfgManagerRef.saveAll();
			}



			responsePtr = nullptr;
			break;
		}

		default:
			// error ?
			// Throw ?
			break;
		}


		return responsePtr;
	}

	void Connection::writeResponse(Packet::MessagePointer msgPtr) {


		// test

		if (msgPtr == nullptr) {
			//std::cerr << "Drop Response" << std::endl;
			return;
		}


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
