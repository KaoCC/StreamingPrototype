#ifndef _SP_CONNECTION_HPP_
#define _SP_CONNECTION_HPP_

#include <memory>
#include <boost/asio.hpp>

#include "Packet.hpp"

#include "../ConfigManager.hpp"
#include "../SyncBuffer.hpp"
#include "../LightField.hpp"

#include <deque>

namespace SP {

	class Connection : public std::enable_shared_from_this<Connection> {

	public:
		using ConnectionPointer = std::shared_ptr<Connection>;


		static ConnectionPointer createWithBuffer(boost::asio::io_service& ios, SyncBuffer<ImageConfig>& buf, LightField& imgLF);
		
		// get socket reference
		boost::asio::ip::tcp::socket& getSocketRef();

		void start();


	private:

		Connection(boost::asio::io_service& io_service, SyncBuffer<ImageConfig>& buf, LightField& imgLF);


		void startReadHeader();
		void handleReadHeader(const boost::system::error_code& error);

		void startReadMessage(size_t msgSize);
		void handleReadMessage(const boost::system::error_code& error);

		void handleWriteMessage(const boost::system::error_code& error);

		Packet::MessagePointer resolvePacket();

		Packet::MessagePointer createResponse(Packet::MessagePointer msgPtr);

		void writeResponse(Packet::MessagePointer msgPtr);


		void appendImage(Packet::DataBuffer& buffer);


		// data members 

		// socket
		boost::asio::ip::tcp::socket streamingSocket;

		// buffers
		Packet::DataBuffer localBuffer;
		//Packet::DataBuffer writeBuffer;

		// packet structure
		Packet packet;
		Packet responsePacket;


		// config
		ConfigManager cfgManager;
		//SyncBuffer<ImageConfig>& bufferRef;

		// tmp
		ImageConfig::ImageBuffer encodedImageData;


		// tmp buffer for async write
		std::deque<Packet::DataBuffer> writeBufferQueue;

	};


    
}





#endif