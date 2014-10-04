// local_peer.h
/*
Provides access from client application to the server side of a locally running peer.
*/

#ifndef __WB_PWORK_LOCAL_PEER_H
#define __WB_PWORK_LOCAL_PEER_H

#include "../shared/job_id.h"
#include "../peerwork/client_id.h"
#include "../shared/tcp_msg_server.h"

#include <boost/asio.hpp>


namespace pwork {

	struct job_description;
	class message;

	class local_peer
	{
	public:
		static std::shared_ptr< local_peer > try_connect(std::string client_name, std::string host, std::string service);

	public:	// TODO: private hack
		local_peer();// tcp::endpoint peer_endpt, client_id cid);
		void disconnect();

	public:
		void register_to(std::string client_name);
		bool post_job(job_description jd);	// - use futures? even possible?
		// query_status(job id)
		// cancel_job(job id)

		void run();

	protected:
		void send_message(message msg);

	protected:
		void on_receive_message(message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn);

	protected:
		void resolve_handler(
			message msg,
			const boost::system::error_code& error,
			tcp::resolver::iterator it
			);
		void connect_handler(
			std::shared_ptr< tcp::socket > sock,
			message msg,
			const boost::system::error_code& error
			);

	private:
		asio::io_service m_io;
		tcp_msg_server m_tcp;
		tcp::endpoint m_remote_endpt;
		client_id m_client_id;
		size_t m_next_job_init_id;
	};

	std::pair< std::string, std::string > find_local_peer();

}


#endif


