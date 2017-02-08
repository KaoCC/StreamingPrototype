#include "Server.hpp"
#include "Connection.hpp"

#include <boost/bind.hpp>

namespace SP {

	Server::Server(boost::asio::io_service & ios, unsigned port) : acc(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {

		startAccept();
	}

	void Server::startAccept() {


		Connection::ConnectionPointer newConnection = Connection::create(acc.get_io_service());

		acc.async_accept(newConnection->getSocketRef(), std::bind(&Server::handleAccept, this, newConnection, std::placeholders::_1));

	}

	void Server::handleAccept(Connection::ConnectionPointer newConnection, const boost::system::error_code& err) {

		if (!err) {
			newConnection->start();
		}


		// again !
		startAccept();
	}


}
