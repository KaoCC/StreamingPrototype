#include "Connection.hpp"


namespace SP {


	Connection::Connection(boost::asio::io_service & ios) : streamingSocket(ios) {
	}

	Connection::ConnectionPointer Connection::create(boost::asio::io_service & ios) {
		return ConnectionPointer(new Connection(ios));
	}

	boost::asio::ip::tcp::socket & Connection::getSocketRef() {
		return streamingSocket;
	}

	void Connection::start() {

	}





}
