// peer.cpp

#include "peer.h"
#include "local_job.h"
#include "../shared/job_description.h"
#include "../shared/message.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <iostream>


namespace pwork {

	// TODO:
	auto const StatusCheckPeriod = std::chrono::milliseconds(5000);

	peer::peer(
		std::string name,
		unsigned short port,
		int max_threads,
		std::vector< tcp::endpoint > peers_to_look_for
		):
		m_timer(m_io_service),
		m_tcp(m_io_service, port, [this](message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn){ on_receive_message(msg, remote, conn); })
	{
		m_pid = boost::uuids::random_generator()();
		m_own_details.name = name;
		m_own_details.endpt = tcp::endpoint{ asio::ip::tcp::v4() /* TODO: !!!??? */, port };
		m_max_threads = max_threads;

		std::cout << "[peer started]" << std::endl;
		std::cout << "name: " << name << std::endl;
		std::cout << "port: " << m_tcp.get_listening_port()/*port*/ << std::endl;
		std::cout << "max threads: " << max_threads << std::endl;
		std::cout << "id: " << m_pid << std::endl << std::endl;

		m_next_job_id = 0;
		m_next_client_id = 100;

		for(auto const& peer_endpt : peers_to_look_for)
		{
			send_ping_to_peer(peer_endpt);
		}

		// Setup timer for periodic status checking
		m_timer.expires_from_now(StatusCheckPeriod);
		m_timer.async_wait(boost::bind(&peer::on_status_check, this));
	}

	job_id peer::generate_job_id()
	{
		return m_next_job_id++;
	}

	local_job peer::generate_job(job_description jd, client_id cid)
	{
		return local_job{ generate_job_id(), std::move(jd), cid };
	}

	peer_list peer::construct_peer_list()
	{
		peer_list pl;

		std::transform(
			std::begin(m_peers),
			std::end(m_peers),
			std::inserter(pl, std::end(pl)),
			[](peer_map_t::value_type const& entry)
		{
			return peer_id_and_details{ entry.first, entry.second };
		});

		return pl;
	}

	bool peer::is_peer_known(peer_id const& pid) const
	{
		return pid == m_pid || m_peers.find(pid) != m_peers.end();
	}

	std::string peer::get_peer_name(peer_id const& pid) const
	{
		if(pid == m_pid)
		{
			return "self";
		}
		else
		{
			return m_peers.at(pid).name;
		}
	}

	peer_id_list peer::check_for_new_peers(peer_list pl)
	{
		peer_id_list unknown;
		for(auto& p : pl)
		{
			if(!is_peer_known(p.id))
			{
				m_peers[p.id] = p;
				unknown.emplace_back(p.id);

				std::cout << "[New peer acquired]" << std::endl;
				std::cout << "name: " << p.name << std::endl;
				std::cout << "location: " << p.endpt.address().to_string() << ":" << p.endpt.port() << std::endl;
				std::cout << "id: " << p.id << std::endl << std::endl;
			}
		}
		return unknown;
	}

	/*
	Posts a new job to the local peer's work queue, for consumption by this or any other peer.
	*/
	void peer::post_job_to_queue(job_id jid)
	{
		m_available_jobs.push_back(jid);

		// NOTE: Now somewhat redundant as advertising available work in the next update ping anyway
		// Actually probably necessary since we are not pinging ourself.
		message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::AvailableWork };
		msg.add_tag< MsgTag::Count >(num_available_jobs());
		send_message_to_all_peers(std::move(msg));
	}

	/*
	Returns whether or not there is available work on this peer's queue
	*/
	bool peer::have_available_work() const
	{
		return !m_available_jobs.empty();
	}

	int peer::num_available_jobs() const
	{
		return m_available_jobs.size();
	}

	/*
	Initiates processing of a job on this peer
	*/
	void peer::start_job(job_id in_progress_jid, job_description jd)
	{
		auto const& type = jd.type;
		auto const& executor = m_executors[type];

		auto update_callback = [this, in_progress_jid](std::string update)
		{
			m_io_service.post([this, in_progress_jid, update]
			{
				on_job_update(in_progress_jid, std::move(update));
			});
		};

		auto completion_callback = [this, in_progress_jid](std::string result)
		{
			m_io_service.post([this, in_progress_jid, result]
			{
				on_job_processing_completed(in_progress_jid, std::move(result));
			});
		};

		m_job_threads.insert(
			std::end(m_job_threads),
			{ in_progress_jid, std::thread{ executor, std::move(jd.specification), update_callback, completion_callback } }
		);
	}

	int peer::num_locally_executing_jobs() const
	{
		return m_executing_jobs.size();
	}

	int peer::num_open_job_requests() const
	{
		int count = 0;
		for(auto const& req : m_open_requests)
		{
			count += req.second.count;
		}
		return count;
	}

	int peer::free_work_capacity() const
	{
		return std::max(m_max_threads - num_locally_executing_jobs() - num_open_job_requests(), 0);
	}

	void peer::look_for_work()
	{
		auto num_free = free_work_capacity();
		if(num_free > 0)
		{
//			std::cout << "looking for work" << std::endl;

			// TODO: Priority order
			auto it = std::begin(m_peer_states);
			while(it != std::end(m_peer_states) && (num_free = free_work_capacity()) > 0)
			{
				auto const& pid = it->first;
				auto& st = it->second;
				if(st.jobs_available > 0)
				{
					auto const num_to_request = std::min(num_free, st.jobs_available);
					st.jobs_available -= num_to_request;
					m_open_requests[pid].count += num_to_request;

					message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::RequestJobs };
					msg.add_tag< MsgTag::Count >(num_to_request);
					send_message_to_peer(pid, std::move(msg));
				}

				++it;
			}
		}
	}

	void peer::check_owned_jobs()
	{
		auto const JobTimeout = std::chrono::seconds(10);

		for(auto const& entry : m_jobs)
		{
			auto jid = entry.first;
			auto& job = entry.second;

			// If job is still queued, or we are executing it ourself, don't worry about it
			if(!job.executing_peer || *job.executing_peer == m_pid)
			{
				continue;
			}

			auto time_since_pinged = std::chrono::system_clock::now() - job.last_ping;
			if(time_since_pinged > JobTimeout)
			{
				// Retract the job
				message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::CancelJob };
				msg.add_tag< MsgTag::JobId >(jid);
				send_message_to_peer(*job.executing_peer, std::move(msg));

				// And put it back on the queue
				post_job_to_queue(jid);
			}
		}
	}
	/*
	void peer::temp_run_job_fn(job_id in_progress_jid, job_description jd)
	{
		// Do the work
		
		auto result = std::string{ "" };
		auto num_terms = std::stoul(jd.specification);
		int t_number = 0;
		for(int term = 1; term <= num_terms; ++term)
		{
			t_number += term;
			result += std::to_string(t_number) + " ";

			std::stringstream ss;
			ss << "Term " << term;
			auto update = ss.str();
			m_io_service.post([this, in_progress_jid, update]
			{
				on_job_update(in_progress_jid, std::move(update));
			});

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		m_io_service.post([this, in_progress_jid, result]
		{
			on_job_processing_completed(in_progress_jid, std::move(result));
		});
	}
	*/
	void peer::send_message_to_client(client_id cid, message msg)
	{
		// Attach ClientId tag on all messages by default, to allow client to check it is not somehow receiving
		// the wrong messages, if it wishes
		msg.add_tag< MsgTag::ClientId >(cid);

		// Attempt to send to the client's last known endpt
		// TODO: C->P message for periodic pings to tell us it is still there. Can optionally include a 
		// ListeningOn tag which will be used to update its endpoint in the client map
		m_tcp.send_message(std::move(msg), m_clients[cid].endpt);
	}

	void peer::send_message_to_peer(peer_id dest_pid, message msg)
	{
		if(dest_pid == m_pid)
		{
			send_message_to_self(std::move(msg));
		}
		else
		{
			msg.add_tag< MsgTag::PeerId >(m_pid);
			m_tcp.send_message(std::move(msg), m_peers[dest_pid].endpt);
		}
	}

	void peer::send_message_to_all_peers(message msg)
	{
		for(auto const& dest : m_peers)
		{
			send_message_to_peer(dest.first, msg);
		}

		// Send to ourself too
		send_message_to_self(std::move(msg));
	}

	void peer::send_message_to_self(message msg)
	{
		m_io_service.post([this, msg]	// TODO: want to move capture msg
		{
			on_receive_peer_message(m_pid, msg.get_type< MessageClass::Peer2Peer, Peer2PeerMsg >(), msg);
		});
	}

	void peer::send_ping_to_peer(peer_id dest_pid)
	{
		message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::Ping };
		msg.add_tag< MsgTag::PeerName >(m_own_details.name);
		msg.add_tag< MsgTag::ListeningOn >(m_own_details.endpt.port());
		msg.add_tag< MsgTag::Count >(num_available_jobs());	// TODO: Better to make AvailableJobs tag?
		msg.add_tag< MsgTag::PeerList >(construct_peer_list());
		for(auto const& executing_job : m_executing_jobs)
		{
			// Is this job being processed on behalf of the destination peer?
			if(executing_job.second.pid == dest_pid)
			{
				msg.add_tag< MsgTag::JobId >(executing_job.second.remote_id);
			}
		}
		send_message_to_peer(dest_pid, std::move(msg));
	}

	void peer::send_ping_to_peer(tcp::endpoint dest_endpt)
	{
		message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::Ping };
		msg.add_tag< MsgTag::PeerName >(m_own_details.name);
		msg.add_tag< MsgTag::ListeningOn >(m_own_details.endpt.port());
		msg.add_tag< MsgTag::Count >(num_available_jobs());	// TODO: Better to make AvailableJobs tag?
		msg.add_tag< MsgTag::PeerList >(construct_peer_list());
		msg.add_tag< MsgTag::PeerId >(m_pid);
		m_tcp.send_message(std::move(msg), dest_endpt);
	}

	void peer::send_update_pings()
	{
		// Ping every known peer
		for(auto const& pr : m_peers)
		{
			auto const& pid = pr.first;
			send_ping_to_peer(pid);
		}
	}

	/*
	This handler processes every message received, from any source.
	*/
	void peer::on_receive_message(message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn)
	{
		switch(msg.get_class())
		{
			case MessageClass::Peer2Peer:
			{
				auto type = msg.get_type< MessageClass::Peer2Peer, Peer2PeerMsg /* TODO: auto */ >();
				auto pid = msg.get< MsgTag::PeerId >();
				if(type == Peer2PeerMsg::Ping)
				{
					auto response_port = msg.get< MsgTag::ListeningOn >();
					tcp::endpoint response_endpt{ remote.address(), (unsigned short)response_port };
					auto pname = msg.get< MsgTag::PeerName >();
					auto pl = msg.get< MsgTag::PeerList >();
					on_receive_ping(pid, pname, response_endpt, std::move(pl), std::move(msg));
				}
				else if(is_peer_known(pid))	// Ignore non-Ping messages if we don't already know the peer
				{
					on_receive_peer_message(pid, type, std::move(msg));
				}
			}
			break;

			case MessageClass::Client2Peer:
			{
				auto type = msg.get_type< MessageClass::Client2Peer, Client2PeerMsg /* TODO: auto */ >();
				if(type == Client2PeerMsg::Register)
				{
					if(msg.has< MsgTag::ClientName >() && msg.has< MsgTag::ListeningOn >())
					{
						auto response_port = msg.get< MsgTag::ListeningOn >();
						tcp::endpoint response_endpt{ remote.address(), (unsigned short)response_port };
						on_receive_client_registration(msg.get< MsgTag::ClientName >(), response_endpt, conn);
					}
				}
				else if(msg.has< MsgTag::ClientId >())
				{
					auto cid = msg.get< MsgTag::ClientId >();
					on_receive_client_message(cid, type, std::move(msg));
				}
			}
			break;

			default:
			// TODO: Report error
			break;
		}
	}

	/*
	First time registration of a client
	*/
	void peer::on_receive_client_registration(std::string name, tcp::endpoint endpt, std::shared_ptr< tcp::socket > conn)
	{
		auto cid = m_next_client_id++;
		m_clients[cid].name = name;
		m_clients[cid].endpt = endpt;

		// Send id back to client on the existing connection
		message response{
			MessageClass::Peer2Client,
			Peer2ClientMsg::RegistrationAccepted
		};
		response.add_tag< MsgTag::ClientId >(cid);
		// TODO: Maybe include some info on number of peers/worker threads, in total and currently free
		m_tcp.send_message(response, conn);//endpt);

		std::cout << "registered new client: { " << name << " @ " << endpt.address().to_string() << ":" <<
			endpt.port() << " }" << std::endl;
	}

	/*
	Deals with any incoming message from an already registered client.
	*/
	void peer::on_receive_client_message(client_id cid, Client2PeerMsg type, message msg)
	{
		switch(type)
		{
			case Client2PeerMsg::Post:
			on_receive_client_job_request(cid, std::move(msg));
			break;

			case Client2PeerMsg::QueryStatus:
			break;

			case Client2PeerMsg::Cancel:
			break;
		}
	}

	void peer::on_receive_client_job_request(client_id cid, message msg)
	{
		auto jd = msg.get< MsgTag::JobDesc >();

		std::cout << "received job request from c[" << m_clients[cid].name << "]: {" << jd.type << " | " << jd.specification << " }" << std::endl;

		// Create a new job
		auto job = generate_job(std::move(jd), cid);
		auto const jid = job.id;
		m_jobs[jid] = std::move(job);

		// Push the job onto the queue of work for this peer
		post_job_to_queue(jid);

		// Associate the job with the client
		m_client_jobs[cid].insert(jid);

		// Reply to the client to confirm that the job has been accepted, and give the job id
		message response{ MessageClass::Peer2Client, Peer2ClientMsg::JobAccepted };
		response.add_tag< MsgTag::JobId >(jid);
		if(msg.has< MsgTag::ClientPrivateData >())
		{
			response.add_tag< MsgTag::ClientPrivateData >(msg.get< MsgTag::ClientPrivateData >());
		}
		send_message_to_client(cid, response);
	}

	/*
	Incoming message from a peer, which could potentially be ourself
	*/
	void peer::on_receive_peer_message(peer_id pid, Peer2PeerMsg type, message msg)
	{
		switch(type)
		{
			case Peer2PeerMsg::AvailableWork:
			{
				auto count = msg.get< MsgTag::Count >();
				on_available_work_notification(pid, count, std::move(msg));
			}
			break;

			case Peer2PeerMsg::RequestJobs:
			{
				auto count = msg.get< MsgTag::Count >();
				on_job_request(pid, count, std::move(msg));
			}
			break;
			
			case Peer2PeerMsg::DelegateJob:
			{
				auto remote_jid = msg.get< MsgTag::JobId >();
				auto jd = msg.get< MsgTag::JobDesc >();
				on_receive_job(pid, remote_jid, std::move(jd), std::move(msg));
			}
			break;

			case Peer2PeerMsg::NoMoreJobs:
			{
				on_notified_no_jobs(pid, std::move(msg));
			}
			break;

			case Peer2PeerMsg::JobUpdate:
			{
				auto jid = msg.get< MsgTag::JobId >();
				on_delegated_job_update(jid, std::move(msg));
			}
			break;

			case Peer2PeerMsg::JobCompleted:
			{
				auto jid = msg.get< MsgTag::JobId >();
				on_delegated_job_completed(jid, std::move(msg));
			}
			break;

			case Peer2PeerMsg::CancelJob:
			{
				// TODO: Allow a list of multiple ids to be cancelled?
				auto jid = msg.get< MsgTag::JobId >();
				on_job_retracted_by_peer(jid, std::move(msg));
			}
			break;
		}
	}

	void peer::on_receive_ping(peer_id pid, std::string pname, tcp::endpoint endpt, peer_list pl, message msg)
	{
		pl.emplace_back(peer_id_and_details{ pid, peer_details{ pname, endpt } });
		auto new_peers = check_for_new_peers(std::move(pl));
		for(auto const& new_pid : new_peers)
		{
			send_ping_to_peer(new_pid);
		}

		// Update the peer's last ping time and available jobs count
		// TODO: peer timeout ... m_peers[pid].last_ping = std::chrono::system_clock::now();
		m_peer_states[pid].jobs_available = msg.get< MsgTag::Count >();

		if(msg.has< MsgTag::JobId >())
		{
			// Update the last ping time for all jobs we own that are being executed by pid
			for(auto const& jid : msg.get_list< MsgTag::JobId >())
			{
				auto& job = m_jobs[jid];

				assert(job.executing_peer && *job.executing_peer == pid);	// TODO: Not assert

				job.last_ping = std::chrono::system_clock::now();
			}
		}

		look_for_work();
	}

	/*
	Invoked when work has become available at pid
	*/
	void peer::on_available_work_notification(peer_id pid, int count, message msg)
	{
		std::cout << "received work available notification from p[" << get_peer_name(pid) << "]" << " (" << count << " jobs)" << std::endl;

		m_peer_states[pid].jobs_available = count;
		look_for_work();
	}

	void peer::on_job_request(peer_id pid, int count, message msg)
	{
		std::cout << "received job request from p[" << get_peer_name(pid) << "]" << " for " << count << " jobs" << std::endl;

		while(count > 0 && have_available_work())
		{
			auto jid = m_available_jobs.front();
			m_available_jobs.pop_front();

			std::cout << "sending job" << std::endl;

			message response{ MessageClass::Peer2Peer, Peer2PeerMsg::DelegateJob };
			// Job id is not unique across all peers, but the peer receiving the job will just store the id used by
			// the owning peer, and pass it back on completion. Note that the peer doing the job does not need to know
			// who the client is, just the id of the owning peer (auto included in all messages) and associated job id.
			response.add_tag< MsgTag::JobId >(jid);
			response.add_tag< MsgTag::JobDesc >(m_jobs[jid].desc);
			send_message_to_peer(pid, std::move(response));

			m_jobs[jid].executing_peer = pid;
			m_jobs[jid].last_ping = std::chrono::system_clock::now();

			--count;
		}

		if(count > 0)
		{
			std::cout << "cannot fully accomodate work request, notifying" << std::endl;

			message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::NoMoreJobs };
			send_message_to_peer(pid, std::move(msg));
		}
	}

	void peer::on_receive_job(peer_id pid, job_id remote_job_id, job_description jd, message msg)
	{
		std::cout << "received job from p[" << get_peer_name(pid) << "]" << std::endl;

		if(num_locally_executing_jobs() < m_max_threads)
		{
			std::cout << "starting job" << std::endl;

			auto const in_progess_jid = m_next_job_id++;
			m_executing_jobs[in_progess_jid] = in_progress_job{ pid, remote_job_id, jd };
			start_job(in_progess_jid, std::move(jd));
		}
		else
		{
			std::cout << "already maxed out at " << num_locally_executing_jobs() << " threads, ignoring" << std::endl;
		}

		// Close the related open request
		m_open_requests[pid].count -= 1;	// TODO: Remove if 0?
	}

	void peer::on_notified_no_jobs(peer_id pid, message msg)
	{
		std::cout << "job unavailable at p[" << get_peer_name(pid) << "]" << std::endl;

		m_open_requests[pid].count -= 1;
		look_for_work();
	}

	void peer::on_delegated_job_update(job_id jid, message msg)
	{
		auto j_it = m_jobs.find(jid);
		assert(j_it != m_jobs.end());
		auto const& job = j_it->second;
		auto cid = job.client;

		message forward{ MessageClass::Peer2Client, Peer2ClientMsg::JobStatus };
		forward.add_tag< MsgTag::JobId >(jid);
		forward.add_tag< MsgTag::JobUpdate >(msg.get< MsgTag::JobUpdate >());
		send_message_to_client(cid, std::move(forward));
	}

	void peer::on_delegated_job_completed(job_id jid, message msg)
	{
		std::cout << "received job completion notification for job id = " << jid << std::endl;

		auto j_it = m_jobs.find(jid);
		assert(j_it != m_jobs.end());
		auto const& job = j_it->second;
		auto cid = job.client;
		m_jobs.erase(jid);
		m_client_jobs[cid].erase(jid);

		std::cout << "notifying client c[" << m_clients[cid].name << "]" << std::endl;

		// Notify client of job completion
		message forward{ MessageClass::Peer2Client, Peer2ClientMsg::JobCompleted };
		forward.add_tag< MsgTag::JobId >(jid);
		// todo: copying tag data from one message to another could maybe be quicker?
		forward.add_tag< MsgTag::JobResult >(msg.get< MsgTag::JobResult >());
		send_message_to_client(cid, std::move(forward));
	}

	void peer::on_job_retracted_by_peer(job_id jid, message msg)
	{
		std::cout << "received job retracted notification for job id = " << jid << std::endl;

		std::cout << "TODO: !!!!!!!!!!!!!" << std::endl;
	}

	void peer::on_job_update(job_id in_progress_jid, std::string update)
	{
		auto j_it = m_executing_jobs.find(in_progress_jid);
		assert(j_it != m_executing_jobs.end());
		auto const& job = j_it->second;

		// Forward on to owning peer
		message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::JobUpdate };
		msg.add_tag< MsgTag::JobId >(job.remote_id);
		msg.add_tag< MsgTag::JobUpdate >(std::move(update));
		send_message_to_peer(job.pid, msg);
	}

	/*
	Methods relating to the job processing aspect of the peer
	*/
	void peer::on_job_processing_completed(job_id in_progress_jid, std::string result)
	{
		std::cout << "completed processing of job, notifying owning peer" << std::endl;

		// Join to the thread that processed the job
		auto t_it = m_job_threads.find(in_progress_jid);
		assert(t_it != m_job_threads.end());
		auto& t = t_it->second;
		t.join();
		m_job_threads.erase(t_it);

		// Notify the peer that owns the job
		auto j_it = m_executing_jobs.find(in_progress_jid);
		assert(j_it != m_executing_jobs.end());
		auto job = std::move(j_it->second);
		m_executing_jobs.erase(j_it);

		message msg{ MessageClass::Peer2Peer, Peer2PeerMsg::JobCompleted };
		msg.add_tag< MsgTag::JobId >(job.remote_id);
		msg.add_tag< MsgTag::JobResult >(result);
		send_message_to_peer(job.pid, msg);

		look_for_work();
	}

	void peer::on_status_check()
	{
		// Send ping to all known peers
		send_update_pings();

		// Check the latest ping times for jobs we own, and deal with any that have gone stale
		check_owned_jobs();

		// Reschedule status check
		m_timer.expires_from_now(StatusCheckPeriod);
		m_timer.async_wait(boost::bind(&peer::on_status_check, this));
	}

	void peer::register_work_type(std::string type_key, peerwork_job_fn executor)
	{
		m_executors[type_key] = executor;

		std::cout << "registering executor: " << type_key << std::endl;
	}

	void peer::run()
	{
		std::cout << "starting" << std::endl;

		// The tcp server is already setup, we just need to start the io service
		m_io_service.run();
	}

}