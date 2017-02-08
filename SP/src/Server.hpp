
#ifndef _SP_SERVER_HPP_
#define _SP_SERVER_HPP_

#include <boost/asio.hpp>

#include "Connection.hpp"

namespace SP {

	class Server {

	public:

		Server(boost::asio::io_service& ios, unsigned port);

	private:

		void startAccept();
		void handleAccept(Connection::ConnectionPointer newConnection, const boost::system::error_code& err);



		boost::asio::ip::tcp::acceptor acc;

		Server() = delete;

	};


}



#endif









