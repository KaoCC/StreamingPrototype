#ifndef _SP_CONNECTION_HPP_
#define _SP_CONNECTION_HPP_

#include <memory>
#include <boost/asio.hpp>

namespace SP {

	class Connection : std::enable_shared_from_this<Connection> {

	public:
		using ConnectionPointer = std::shared_ptr<Connection>;


		static ConnectionPointer create(boost::asio::io_service& ios);
		
		// get socket ref ?

		boost::asio::ip::tcp::socket& getSocketRef();

		void start();


	private:

		Connection(boost::asio::io_service& io_service);



		boost::asio::ip::tcp::socket streamingSocket;

		// buffers ?
	};


    
}





#endif