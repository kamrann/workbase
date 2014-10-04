// tcp_msg_server.h

#ifndef __WB_PWORK_TCP_H
#define __WB_PWORK_TCP_H

#include "tcp_incoming_msg.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <array>
#include <functional>
#include <string>
#include <tuple>


namespace pwork {

	namespace asio = boost::asio;
	using boost::asio::ip::tcp;

	class message;

	class tcp_msg_server
	{
	public:
		tcp_msg_server(asio::io_service& io_service, int port, message_dispatch_fn on_msg);

		unsigned short get_listening_port() const;

		typedef std::function< void(tcp::endpoint resolved_endpt) > on_connect_fn;

		std::shared_ptr< tcp::socket > open_connection(std::string host, std::string service);
		bool send_message_sync(message msg, tcp::socket& sock);
		message receive_message_sync(tcp::socket& sock);
		
		void send_message(message msg, tcp::endpoint remote);
		void send_message(message msg, std::shared_ptr< tcp::socket > open_conn);

	private:
		// Receiving
		void start_accept();

		void accept_handler(
			tcp_incoming_msg::pointer incoming,
			const boost::system::error_code& error
			);

		// Transmitting
		void resolve_handler(
			message msg,
			on_connect_fn on_connect,
			const boost::system::error_code& error,
			tcp::resolver::iterator it
			);
		void connect_handler(
			std::shared_ptr< tcp::socket > sock,
			message msg,
			on_connect_fn on_connect,
			const boost::system::error_code& error
			);

	private:
		tcp::acceptor m_acceptor;
		tcp::resolver m_resolver;
		message_dispatch_fn m_on_msg;
	};

}



#endif


