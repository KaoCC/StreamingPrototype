
#ifndef _SP_SERVER_HPP_
#define _SP_SERVER_HPP_

#include <boost/asio.hpp>

#include "Connection.hpp"

#include "../SyncBuffer.hpp"
#include "Common.hpp"


namespace SP {

	class Server {

	public:

		Server() = delete;
		Server(boost::asio::io_service& ios, unsigned port, SyncBuffer<ImageConfig>& syncBuff, LightField& imgLF);

	private:

		void startAccept();
		void handleAccept(Connection::ConnectionPointer newConnection, const boost::system::error_code& err);


		// acceptor
		boost::asio::ip::tcp::acceptor acc;
		SyncBuffer<ImageConfig>& syncBufferRef;
		LightField& imgLightFieldRef;

	};


}



#endif









