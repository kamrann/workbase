// tcp_incoming_msg.cpp

#include "tcp_incoming_msg.h"
#include "message.h"


namespace pwork {

	tcp_incoming_msg::pointer tcp_incoming_msg::create(asio::io_service& io_service, message_dispatch_fn on_msg)
	{
		return std::make_shared< tcp_incoming_msg >(io_service, on_msg, ctr_key{});
	}

	void tcp_incoming_msg::start_receive(pointer ptr)
	{
		// Read the number of bytes representing the data size marker
		ptr->m_read_msg_size = false;
		ptr->m_read_buffer.resize(MessageSizeMarkerBytes);
		asio::async_read(ptr->m_socket, asio::buffer(ptr->m_read_buffer),
			boost::bind(&tcp_incoming_msg::read_handler, ptr,
			asio::placeholders::error,
			asio::placeholders::bytes_transferred));

//		ptr->async_read_to_buffer(ptr);
	}

	tcp::socket& tcp_incoming_msg::socket()
	{
		return m_socket;
	}

	tcp::endpoint& tcp_incoming_msg::remote()
	{
		return m_remote;
	}


	tcp_incoming_msg::tcp_incoming_msg(asio::io_service& io_service, message_dispatch_fn on_msg, ctr_key):
		m_socket{ io_service },
		m_read_buffer{},
		m_on_msg{ on_msg }
	{

	}

	void tcp_incoming_msg::read_handler(
		pointer ptr,
		const boost::system::error_code &ec,
		std::size_t bytes_transferred
		)
	{
		if(!ec)
		{
			if(!ptr->m_read_msg_size)
			{
				assert(bytes_transferred == MessageSizeMarkerBytes);

				ptr->m_read_msg_size = true;
				auto msg_size = *(size_t*)&ptr->m_read_buffer[0];
				ptr->m_read_buffer.resize(msg_size);
				asio::async_read(ptr->m_socket, asio::buffer(ptr->m_read_buffer),
					boost::bind(&tcp_incoming_msg::read_handler, ptr,
					asio::placeholders::error,
					asio::placeholders::bytes_transferred));
			}
			else
			{
				// Already know the message size, so must have just read the message
				ptr->append_to_msg_buffer(bytes_transferred);
				ptr->dispatch_msg();
			}

//			ptr->append_to_msg_buffer(bytes_transferred);
//			ptr->async_read_to_buffer(ptr);
		}
		else
		{
			// Assuming connection closed by sender
//			ptr->dispatch_msg();
		}
	}

	void tcp_incoming_msg::async_read_to_buffer(pointer ptr)
	{
		m_socket.async_read_some(asio::buffer(m_read_buffer),
			boost::bind(&tcp_incoming_msg::read_handler, ptr,
			asio::placeholders::error,
			asio::placeholders::bytes_transferred));
	}

	void tcp_incoming_msg::append_to_msg_buffer(size_t bytes)
	{
		m_msg_buffer += std::string{ std::begin(m_read_buffer), std::begin(m_read_buffer) + bytes };
	}

	void tcp_incoming_msg::dispatch_msg()
	{
		// Check we've received a full valid transmission
		auto msg = extract_message(m_msg_buffer);
		if(msg)
		{
			m_on_msg(msg, m_remote, std::make_shared< tcp::socket >(std::move(m_socket)));
		}
		else
		{
			// TODO: ?? failure
		}
	}

}

