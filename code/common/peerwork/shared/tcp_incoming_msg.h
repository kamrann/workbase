// tcp_incoming_msg.h

#ifndef __WB_PWORK_TCP_INCOMING_MSG_H
#define __WB_PWORK_TCP_INCOMING_MSG_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <array>
#include <functional>


namespace pwork {

	namespace asio = boost::asio;
	using boost::asio::ip::tcp;

	class message;

	typedef std::function< void(message, tcp::endpoint, std::shared_ptr< tcp::socket >) > message_dispatch_fn;

	class tcp_incoming_msg
	{
	public:
		typedef std::shared_ptr< tcp_incoming_msg > pointer;

		static pointer create(asio::io_service& io_service, message_dispatch_fn on_msg);
		static void start_receive(pointer ptr);

		tcp::socket& socket();
		tcp::endpoint& remote();

	private:
		static void read_handler(
			pointer ptr,
			const boost::system::error_code &ec,
			std::size_t bytes_transferred
			);

		void async_read_to_buffer(pointer ptr);
		void append_to_msg_buffer(size_t bytes);
		void dispatch_msg();

	private:
		struct ctr_key {};

		tcp_incoming_msg(const tcp_incoming_msg&) = delete;
		const tcp_incoming_msg& operator= (const tcp_incoming_msg&) = delete;

	public:
		explicit tcp_incoming_msg(asio::io_service& io_service, message_dispatch_fn on_msg, ctr_key);

	private:
		tcp::socket m_socket;
//		std::array< char, 4096 > m_read_buffer;	// TODO: Size
		std::vector< char > m_read_buffer;
		std::string m_msg_buffer;
		bool m_read_msg_size;
		message_dispatch_fn m_on_msg;
		tcp::endpoint m_remote;
	};

}



#endif


