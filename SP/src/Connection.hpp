#ifndef _SP_CONNECTION_HPP_
#define _SP_CONNECTION_HPP_

#include <memory>
#include <boost/asio.hpp>

#include "Packet.hpp"

#include "ConfigManager.hpp"

namespace SP {

	class Connection : std::enable_shared_from_this<Connection> {

	public:
		using ConnectionPointer = std::shared_ptr<Connection>;


		static ConnectionPointer create(boost::asio::io_service& ios);
		
		// get socket reference
		boost::asio::ip::tcp::socket& getSocketRef();

		void start();


	private:

		Connection(boost::asio::io_service& io_service);


		void startReadHeader();
		void handleReadHeader(const boost::system::error_code& error);

		void startReadMessage(size_t msgSize);
		void handleReadMessage(const boost::system::error_code& error);


		Packet::MessagePointer resolvePacket();

		Packet::MessagePointer createResponse(Packet::MessagePointer msgPtr);

		void writeRseponse(Packet::MessagePointer msgPtr);


		// data members 

		// socket
		boost::asio::ip::tcp::socket streamingSocket;

		// buffers
		Packet::DataBuffer localBuffer;

		// packet structure
		Packet packet;


		// config
		ConfigManager cfgManager;

	};


    
}





#endif