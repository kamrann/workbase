// tcp_msg_server.cpp

#include "tcp_msg_server.h"
#include "tcp_incoming_msg.h"
#include "message.h"


namespace pwork {

	tcp_msg_server::tcp_msg_server(asio::io_service& io_service, int port, message_dispatch_fn on_msg):
		m_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		m_resolver(io_service),
		m_on_msg(on_msg)
	{
		start_accept();
	}

	unsigned short tcp_msg_server::get_listening_port() const
	{
		return m_acceptor.local_endpoint().port();
	}

	std::shared_ptr< tcp::socket > tcp_msg_server::open_connection(std::string host, std::string service)
	{
		boost::system::error_code ec;
		auto ep_it = m_resolver.resolve(tcp::resolver::query{ host, service }, ec);
		if(ec != 0)
		{
			return nullptr;
		}

		auto sock = std::make_shared< tcp::socket >(m_acceptor.get_io_service());
		ep_it = asio::connect(*sock, ep_it);
		if(ep_it == tcp::resolver::iterator{})
		{
			return nullptr;
		}

		return sock;
	}

	bool tcp_msg_server::send_message_sync(message msg, tcp::socket& sock)
	{
		// Ready to transmit. Encode the message.
		auto msg_buffer = encode_message(msg);

		// Write
		auto sent = asio::write(sock, asio::buffer(msg_buffer));
		if(sent != msg_buffer.size() * sizeof(msg_buffer[0]))
		{
			return false;
		}

		return true;
	}

	message tcp_msg_server::receive_message_sync(tcp::socket& sock)
	{
		message msg;

		std::array< char, MessageSizeMarkerBytes > size_marker_buf;
		auto count = asio::read(sock, asio::buffer(size_marker_buf));
		if(count != MessageSizeMarkerBytes)
		{
			return msg;
		}

		// TODO: Portable way?
		auto msg_size = *(size_t*)&size_marker_buf[0];

		std::vector< char > msg_buf;
		msg_buf.resize(msg_size);
		count = asio::read(sock, asio::buffer(msg_buf));
		if(count != msg_size)
		{
			return msg;
		}

		std::string msg_str{ std::begin(msg_buf), std::end(msg_buf) };
		msg = extract_message(msg_str);
		return msg;
	}
	/*
	void tcp_msg_server::send_message(message msg, std::string host, std::string service, on_connect_fn on_connect)
	{
		m_resolver.async_resolve(
			tcp::resolver::query{ host, service }, boost::bind(
			&tcp_msg_server::resolve_handler,
			this,
			msg,
			on_connect,
			asio::placeholders::error,
			asio::placeholders::iterator
		));
	}
	*/
	void tcp_msg_server::send_message(message msg, tcp::endpoint remote)
	{
		auto sock = std::make_shared< tcp::socket >(m_acceptor.get_io_service());
		sock->async_connect(remote, boost::bind(&tcp_msg_server::connect_handler, this,
			sock,
			msg,
			on_connect_fn{},
			asio::placeholders::error
			));
	}

	void tcp_msg_server::send_message(message msg, std::shared_ptr< tcp::socket > open_conn)
	{
		connect_handler(open_conn, msg, on_connect_fn{}, boost::system::error_code{});
	}

	/*
	Receiving
	*/
	void tcp_msg_server::start_accept()
	{
		auto incoming = tcp_incoming_msg::create(m_acceptor.get_io_service(), m_on_msg);

		m_acceptor.async_accept(
			incoming->socket(),
			incoming->remote(),
			boost::bind(&tcp_msg_server::accept_handler, this, incoming,
			asio::placeholders::error));
	}

	void tcp_msg_server::accept_handler(
		tcp_incoming_msg::pointer incoming,
		const boost::system::error_code& error
		)
	{
		if(!error)
		{
			tcp_incoming_msg::start_receive(incoming);
		}

		start_accept();
	}

	/*
	Transmitting
	*/
	void tcp_msg_server::resolve_handler(
		message msg,
		on_connect_fn on_connect,
		const boost::system::error_code& error,
		tcp::resolver::iterator it
		)
	{
		if(!error)
		{
			auto sock = std::make_shared< tcp::socket >(m_acceptor.get_io_service());
			asio::async_connect(*sock, it, boost::bind(
				&tcp_msg_server::connect_handler,
				this,
				sock,
				msg,
				on_connect_fn{},
				asio::placeholders::error
				));
		}
	}

	void tcp_msg_server::connect_handler(
		std::shared_ptr< tcp::socket > sock,
		message msg,
		on_connect_fn on_connect,
		const boost::system::error_code& error
		)
	{
		if(!error)
		{
			if(on_connect)
			{
				on_connect(sock->remote_endpoint());
			}

			// Ready to transmit. Encode the message onto a shared_ptr managed buffer
			auto msg_buffer = encode_message(msg);
			auto buffer_ptr = std::make_shared< std::vector< char > >(std::move(msg_buffer));

			// Initiate the asynchronous write
			asio::async_write(
				*sock,
				asio::buffer(*buffer_ptr),
				// Handler not required, but capture socket and buffer so that they won't be
				// destroyed until after the write completes.
				[sock, buffer_ptr](const boost::system::error_code&, size_t){}
			);
		}
	}

}

