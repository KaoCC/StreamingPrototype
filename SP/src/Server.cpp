#include "Server.hpp"
#include "Connection.hpp"

namespace SP {

	Server::Server(boost::asio::io_service & ios, unsigned port) : acc(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {

		startAccept();
	}

	void Server::startAccept() {


		Connection::ConnectionPointer newConnection = Connection::create(acc.get_io_service());



	}

	void Server::handleAccept() {
	}

}
