#include "Connection.hpp"

#include <iostream>

#include <thread>
#include <chrono>

namespace SP {


	Connection::ConnectionPointer Connection::create(boost::asio::io_service & ios, ConfigManager& configRef) {
		return ConnectionPointer(new Connection(ios, configRef));
	}

	Connection::Connection(boost::asio::io_service & ios, ConfigManager& configRef) :
		streamingSocket(ios), packet(Packet::MessagePointer(new StreamingFormat::StreamingMessage())), mCfgManagerRef(configRef),
		responsePacket(Packet::MessagePointer(new StreamingFormat::StreamingMessage())), mEncoder(CreateEncoder(configRef.getScreenWidth(), configRef.getScreenHeight())) {
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

			const auto& responseVector{ createResponse(msgPtr) };

			for (const auto& response : responseVector) {
				writeResponse(response);
			}

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

	std::vector<Packet::MessagePointer> Connection::createResponse(Packet::MessagePointer msgPtr) {


		std::vector<Packet::MessagePointer> responseVector;

		// KAOCC: TODO: prevent creating new Messages for optimization
		//Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };

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

			Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };

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

			responseVector.push_back(responsePtr);

			break;

		}

		case StreamingFormat::MessageType::MsgDefaultPos:
		{
			// Possible Error
			// The server will not get this one!
			break;
		}

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
			if (writeBufferQueue.size() > mCfgManagerRef.getWriteBufferSize()) {
				//responsePtr = nullptr;
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


			uint8_t* rawPtr = mEncoder->getEncoderRawBuffer();

			// TMP !!!
			//size_t subLFIndex = mCfgManagerRef.getIndexOfSubLightField(dx);

			auto& lightFieldRef = mCfgManagerRef.getLightField();

			const auto& indexArray{ mCfgManagerRef.getIndexArrayOfSubLightField(dx) };

			size_t subLFSz = mCfgManagerRef.getNumberOfSubLFImages();

			encodedDataQueue.clear();

			for (const std::size_t subLFIndex : indexArray) {


				if (!lightFieldRef[subLFIndex].getRefreshState()) {
					//std::cerr << "No need to send\n";
					//responsePtr = nullptr;
					continue;
				} else {
					std::cerr << "Get new: " << subLFIndex << "\n";
				}

				Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };

				// reply the images
				responsePtr->set_type(StreamingFormat::MessageType::MsgImage);

				// test
				//encodedDataVector.clear();

				ImageConfig::ImageBuffer encodedImageData;

				for (size_t k = 0; k < subLFSz; ++k) {

					// need to optimize for copying !
					//ImageConfig imageData{ cfgManager.getImage() };
					//ImageConfig::ImageBuffer& imageBufferCache{ imageData.getImageData() };

					const ImageConfig::ImageBuffer& imageBufferCache{ lightFieldRef[subLFIndex][k].getImageData() };

					std::copy(imageBufferCache.cbegin(), imageBufferCache.cend(), rawPtr);

					uint8_t* outBufPtr = nullptr;
					int outSize = 0;
					mEncoder->startEncoding(&outBufPtr, &outSize);

					//ImageConfig::ImageBuffer encodedImageData;
					if (outSize > 0) {
						encodedImageData.insert(std::end(encodedImageData), outBufPtr, outBufPtr + outSize);

					} else {
						std::cerr << "Failed to Encode !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << '\n';
					}

					//using namespace std::chrono_literals;
					//std::this_thread::sleep_for(1s);

				}


				StreamingFormat::Image* imagePtr{ new StreamingFormat::Image };

				// for testing only
				imagePtr->set_serialnumber(serialNumber);

				//std::cerr << "size" << encodedImageData.size() << std::endl;
				imagePtr->set_bytesize(encodedImageData.size()); // tmp
				//imagePtr->set_status(imageData.getID());  // tmp

				imagePtr->set_status(subLFIndex);	// test

				// image data ????
				// need to check
				//imagePtr->set_imagedata(reinterpret_cast<const char*>(imageData.getImageRawData()));

				encodedDataQueue.push_back(std::move(encodedImageData));


				responsePtr->set_allocated_imagemsg(imagePtr);

				responseVector.push_back(responsePtr);

			}

			break;
		}

		case StreamingFormat::MessageType::MsgImage:
		{
			// Possible error
			// The server will not get this one !
			break;
		}

		case StreamingFormat::MessageType::MsgEnding:
		{

			Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };

			// tmp: for testing only
			// Echo the Ending msg back ?
			responsePtr->set_type(StreamingFormat::MessageType::MsgEnding);
			responseVector.push_back(responsePtr);

			// TODO:
			// Close the connection !

			break;
		}

		case StreamingFormat::MessageType::MsgControl:
		{
			// test
			std::cerr << "control msg\n";

			const auto& changeFlag = msgPtr->controlmsg().change_scene();
			if (changeFlag) {		// != 0
				// tmp
				std::cerr << "clear Scene\n";

				mCfgManagerRef.setAllSceneChangedFlag(true);
				//mCfgManagerRef.clearAll();
			}


			const auto& saveFlag = msgPtr->controlmsg().save_frame();
			if (saveFlag) {  // != 0
				std::cerr << "Save Images !\n";
				mCfgManagerRef.saveAll();
			}

			if (msgPtr->controlmsg().has_editingmsg()) {
				const auto&  editingMsg = msgPtr->controlmsg().editingmsg();
				// op is enum
				// StreamingFormat::EditOperation.START(0)/FINISH(1)/UPDATE(2)


				
				// reset test
				switch (editingMsg.op()) {
				case StreamingFormat::EditOperation::START:
					std::cerr << "Editing START:" << std::endl;
					writeModelIdList();
					mCfgManagerRef.resetRenderer();
					break;
				case StreamingFormat::EditOperation::FINISH:
					std::cerr << "Editing FINISH:" << std::endl;
					break;
				case StreamingFormat::EditOperation::UPDATE:
					std::cerr << "Editing UPDATE:" << editingMsg.op() << ", screen X: " << editingMsg.screen_x() << ", screen Y: " << editingMsg.screen_y() << std::endl;
					mCfgManagerRef.recompileScene();
					break;
				case StreamingFormat::EditOperation::SET_MODEL_ID:
					std::cerr << "Change Model ID:"  << editingMsg.model_id() << std::endl;
					break;
				}


			}






			//responsePtr = nullptr;
			break;
		}

		default:
			// error ?
			// Throw ?
			break;
		}


		return responseVector;
	}

	void Connection::writeModelIdList() {
		Packet::MessagePointer responsePtr{ new StreamingFormat::StreamingMessage };
		StreamingFormat::Control* controlPtr{ new StreamingFormat::Control };
		StreamingFormat::Editing* editPtr{ new StreamingFormat::Editing };

		editPtr->set_op(StreamingFormat::EditOperation::MODEL_LIST);
		
		// TODO: for test only
		
		int nModels = rand() % 15 + 1;
		for (int i = 0;i < nModels;i++) {
			editPtr->add_model_ids(i);
		}


		controlPtr->set_allocated_editingmsg(editPtr);

		responsePtr->set_type(StreamingFormat::MessageType::MsgControl);
		responsePtr->set_allocated_controlmsg(controlPtr);


		writeResponse(responsePtr);
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


					// KAOCC: Need to change ! Not a good approach !
				appendImage(writeBuffer, encodedDataQueue.front());
				encodedDataQueue.pop_front();

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
	void Connection::appendImage(Packet::DataBuffer & buffer, const ImageConfig::ImageBuffer& encodedData) {


		buffer.insert(std::end(buffer), std::begin(encodedData), std::end(encodedData));

		// test
		//std::cerr << "insert size " << buffer.size() << '\n';
	}



	boost::asio::ip::tcp::socket & Connection::getSocketRef() {
		return streamingSocket;
	}






}
