// peer.h

#ifndef __WB_PWORK_PEER_H
#define __WB_PWORK_PEER_H

#include "peer_id.h"
#include "client_id.h"
#include "local_job.h"
#include "in_progress_job.h"
#include "peer_state.h"
#include "open_request.h"
#include "../shared/job_id.h"
#include "../shared/message.h"
#include "../shared/tcp_msg_server.h"

#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>

#include <map>
#include <set>
#include <deque>
#include <thread>
#include <functional>


namespace pwork {

	struct job_description;
	struct local_job;

	typedef std::function< void(std::string) > job_update_callback;
	typedef std::function< void(std::string) > job_completion_callback;
	typedef std::function< void(std::string, job_update_callback, job_completion_callback) > peerwork_job_fn;

	class peer
	{
	public:
		peer(
			std::string name,
			unsigned short port,
			int max_threads,
			std::vector< tcp::endpoint > peers_to_look_for
			);

	public:
		void register_work_type(std::string type_key, peerwork_job_fn executor);
		void run();

	protected:
		// TODO: Ideall make these take just a P2C message type, and variadic list of tag values
		void send_message_to_client(client_id cid, message msg);
		void send_message_to_peer(peer_id dest_pid, message msg);
		void send_message_to_all_peers(message msg);
		void send_message_to_self(message msg);
		void send_ping_to_peer(peer_id dest_pid);
		void send_ping_to_peer(tcp::endpoint dest_endpt);
		void send_update_pings();

	protected:
		job_id generate_job_id();
		local_job generate_job(job_description jd, client_id cid);
		peer_list construct_peer_list();// bool include_self);
		bool is_peer_known(peer_id const& pid) const;
		std::string get_peer_name(peer_id const& pid) const;
		peer_id_list check_for_new_peers(peer_list pl);
		void post_job_to_queue(job_id jid);
		bool have_available_work() const;
		int num_available_jobs() const;
		void start_job(job_id in_progress_jid, job_description jd);
		int num_locally_executing_jobs() const;
		int num_open_job_requests() const;
		int free_work_capacity() const;
		void look_for_work();
		void check_owned_jobs();

//		void temp_run_job_fn(job_id in_progress_jid, job_description jd);

	protected:
		void on_receive_message(message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn);

		void on_receive_client_registration(std::string name, tcp::endpoint endpt, std::shared_ptr< tcp::socket > conn);
		void on_receive_client_message(client_id cid, Client2PeerMsg type, message msg);
		void on_receive_client_job_request(client_id cid, message msg);
		void on_receive_client_status_request();
		void on_receive_client_cancel_request();

		void on_receive_peer_message(peer_id pid, Peer2PeerMsg type, message msg);

		void on_receive_ping(peer_id pid, std::string pname, tcp::endpoint endpt, peer_list pl, message msg);
		void on_available_work_notification(peer_id pid, int count, message msg);
		void on_job_request(peer_id pid, int count, message msg);
		void on_receive_job(peer_id pid, job_id remote_job_id, job_description jd, message msg);
		void on_notified_no_jobs(peer_id pid, message msg);
		void on_delegated_job_update(job_id jid, message msg);
		void on_delegated_job_completed(job_id jid, message msg);
		void on_job_retracted_by_peer(job_id jid, message msg);

		// Local job processing
		void on_job_update(job_id in_progress_jid, std::string update);
		void on_job_processing_completed(job_id in_progress_jid, std::string result);

		// Timer
		void on_status_check();

	protected:
		typedef std::map< job_id, local_job > job_map_t;
		typedef std::map< client_id, client_details > client_details_map_t;
		typedef std::set< job_id > client_job_set_t;
		typedef std::map< client_id, client_job_set_t > client_job_map_t;
		typedef std::map< peer_id, peer_details > peer_map_t;
		typedef std::deque< job_id > job_queue_t;
		typedef std::map< job_id, in_progress_job > in_progress_map_t;
		typedef std::map< peer_id, open_request > open_request_map_t;
		typedef std::map< peer_id, peer_state > peer_state_map_t;
		typedef std::map< std::string, peerwork_job_fn > peerwork_executor_map_t;

	protected:
		peer_id m_pid;
		peer_details m_own_details;
		int m_max_threads;
		job_id m_next_job_id;
		client_id m_next_client_id;
		job_map_t m_jobs;
		client_details_map_t m_clients;
		client_job_map_t m_client_jobs;
		job_queue_t m_available_jobs;
		in_progress_map_t m_executing_jobs;	// Locally executing
		std::map< job_id, std::thread > m_job_threads;
		peer_map_t m_peers;
		peer_state_map_t m_peer_states;
		open_request_map_t m_open_requests;
		peerwork_executor_map_t m_executors;

		boost::asio::io_service m_io_service;
		boost::asio::system_timer m_timer;
		tcp_msg_server m_tcp;
	};

}


#endif


