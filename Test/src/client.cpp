
#include <iostream>
#include <boost/asio.hpp>

#include "message.pb.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {


	try {


		boost::asio::io_service ios;
		tcp::resolver res(ios);

		const std::string ip = "127.0.0.1";
		const std::string port = "8051";

		tcp::resolver::query quer(ip, port);
		tcp::resolver::iterator endpoint_iterator = res.resolve(quer);



		tcp::socket streamingSocket(ios);
		boost::asio::connect(streamingSocket, endpoint_iterator);

		// handle data below
		//while (true) {

		//}

		boost::system::error_code error;

		// allocate on the heap
		StreamingFormat::Init* init = new StreamingFormat::Init;
		init->set_width(1024);
		init->set_height(768);
		init->set_moduleid(10);



		// ...


	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}


	return 0;
}