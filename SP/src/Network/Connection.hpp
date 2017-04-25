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


		static ConnectionPointer create(boost::asio::io_service& ios, ConfigManager& configRef);
		
		// get socket reference
		boost::asio::ip::tcp::socket& getSocketRef();

		void start();

	private:

		Connection(boost::asio::io_service& io_service, ConfigManager& configRef);


		void startReadHeader();
		void handleReadHeader(const boost::system::error_code& error);

		void startReadMessage(size_t msgSize);
		void handleReadMessage(const boost::system::error_code& error);

		void handleWriteMessage(const boost::system::error_code& error);

		Packet::MessagePointer resolvePacket();

		std::vector<Packet::MessagePointer> createResponse(Packet::MessagePointer msgPtr);

		void writeResponse(Packet::MessagePointer msgPtr);


		void appendImage(Packet::DataBuffer& buffer, const ImageConfig::ImageBuffer& encodedData);



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
		ConfigManager& mCfgManagerRef;

		//SyncBuffer<ImageConfig>& bufferRef;

		// tmp
		//ImageConfig::ImageBuffer encodedImageData;

		std::deque<ImageConfig::ImageBuffer> encodedDataVector;


		// tmp buffer for async write
		std::deque<Packet::DataBuffer> writeBufferQueue;


		// Encoder
		std::unique_ptr<Encoder> mEncoder;

	};


    
}





#endif