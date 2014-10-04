// local_peer.cpp

#include "local_peer.h"
#include "../shared/message.h"

#include <iostream>


namespace pwork {

	//const int ListeningPort = 22;	// TODO:

	std::shared_ptr< local_peer > local_peer::try_connect(std::string client_name, std::string host, std::string service)
	{
		// TODO: hack. for sync methods, tcp_msg_server doesn't need an io service...
		// Need to have encapsulation of sync methods only
		auto lp = std::make_shared< local_peer >();
		auto listening_port = lp->m_tcp.get_listening_port();

		auto conn = lp->m_tcp.open_connection(host, service);
		if(!conn)
		{
			return nullptr;
		}

		message msg{
			MessageClass::Client2Peer,
			Client2PeerMsg::Register
		};
		msg.add_tag< MsgTag::ClientName >(client_name);
		msg.add_tag< MsgTag::ListeningOn >(listening_port);

		auto success = lp->m_tcp.send_message_sync(msg, *conn);
		if(!success)
		{
			return nullptr;
		}

		auto response = lp->m_tcp.receive_message_sync(*conn);
		if(!response)
		{
			return nullptr;
		}

		if(response.get_class() != MessageClass::Peer2Client)
		{
			return nullptr;
		}

		auto type = response.get_type< MessageClass::Peer2Client, Peer2ClientMsg >();
		if(type != Peer2ClientMsg::RegistrationAccepted)
		{
			return nullptr;
		}

		lp->m_remote_endpt = conn->remote_endpoint();
		lp->m_client_id = response.get< MsgTag::ClientId >();

		std::cout << "Registration confirmation received, client id = " << lp->m_client_id << std::endl;

		return lp;
	}

	local_peer::local_peer()://tcp::endpoint peer_endpt, client_id cid):
		m_tcp{ m_io, 0, [this](message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn){ on_receive_message(msg, remote, conn); } }
	{
		m_next_job_init_id = 0;
	}

	void local_peer::disconnect()
	{
		// TODO: clean disconnect from peer
		m_io.stop();
	}

	void local_peer::send_message(message msg)
	{
		msg.add_tag< MsgTag::ClientId >(m_client_id);
		m_tcp.send_message(msg, m_remote_endpt);
	}
	/*
	void local_peer::register_to(std::string client_name)
	{
		message msg{
			MessageClass::Client2Peer,
			Client2PeerMsg::Register
		};
		msg.add_tag< MsgTag::ClientName >(client_name);
		msg.add_tag< MsgTag::ListeningOn >(ListeningPort);
		send_message(std::move(msg));
	}
	*/

	bool local_peer::post_job(job_description jd)
	{
		message msg{
			MessageClass::Client2Peer,
			Client2PeerMsg::Post
		};
		msg.add_tag< MsgTag::JobDesc >(std::move(jd));
		msg.add_tag< MsgTag::ClientPrivateData >(std::to_string(m_next_job_init_id++));
		send_message(std::move(msg));
		return true;
	}

	void local_peer::run()
	{
		m_io.run();
	}

	void local_peer::on_receive_message(message msg, tcp::endpoint remote, std::shared_ptr< tcp::socket > conn)
	{
		assert(msg.get_class() == MessageClass::Peer2Client);

		switch(msg.get_type< MessageClass::Peer2Client, Peer2ClientMsg >())
		{
			case Peer2ClientMsg::JobAccepted:
			{
				std::cout << "Job accepted with id = " << msg.get< MsgTag::JobId >() << std::endl;
			}
			break;

			case Peer2ClientMsg::JobStatus:
			{
				std::cout << "Job [id = " << msg.get< MsgTag::JobId >() << "] update:" << std::endl;
				std::cout << msg.get< MsgTag::JobUpdate >() << std::endl;
			}
			break;

			case Peer2ClientMsg::JobCompleted:
			{
				std::cout << "Job [id = " << msg.get< MsgTag::JobId >() << "] completed with result:" << std::endl;
				std::cout << msg.get< MsgTag::JobResult >() << std::endl;
			}
			break;
		}
	}


	std::pair< std::string, std::string > find_local_peer()
	{
		return{ "127.0.0.1", "13" };
	}

}


