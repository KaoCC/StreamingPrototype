#include "Connection.hpp"


namespace SP {

	Connection::ConnectionPointer Connection::create(boost::asio::io_service & ios) {
		return ConnectionPointer(new Connection(ios));
	}




}
