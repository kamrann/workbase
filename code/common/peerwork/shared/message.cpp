// message.cpp

#include "message.h"
#include "message_yaml.h"


namespace pwork {

	bimap< MessageClass, std::string > const msg_cls_traits::mp = {
			{ MessageClass::Peer2Peer, "p2p" },
			{ MessageClass::Peer2Client, "p2c" },
			{ MessageClass::Client2Peer, "c2p" },
	};

	bimap< Peer2PeerMsg, std::string > const p2p_msg_traits::mp = {
			{ Peer2PeerMsg::Ping, "ping" },
			{ Peer2PeerMsg::AvailableWork, "avail_wk" },
			{ Peer2PeerMsg::RequestJobs, "request_jobs" },
			{ Peer2PeerMsg::DelegateJob, "delegate_job" },
			{ Peer2PeerMsg::NoMoreJobs, "no_more_jobs" },
			{ Peer2PeerMsg::JobCompleted, "job_done" },
			{ Peer2PeerMsg::CancelJob, "cancel_job" },
			{ Peer2PeerMsg::JobUpdate, "job_update" },
	};

	bimap< Peer2ClientMsg, std::string > const p2c_msg_traits::mp = {
			{ Peer2ClientMsg::RegistrationAccepted, "registered" },
			{ Peer2ClientMsg::JobAccepted, "accepted" },
			{ Peer2ClientMsg::JobStatus, "status" },
			{ Peer2ClientMsg::JobCompleted, "completed" },
	};

	bimap< Client2PeerMsg, std::string > const c2p_msg_traits::mp = {
			{ Client2PeerMsg::Register, "reg" },
			{ Client2PeerMsg::Post, "post" },
			{ Client2PeerMsg::QueryStatus, "query" },
			{ Client2PeerMsg::Cancel, "cancel" },
	};


	message::message():
		m_cls{ MessageClass::Undefined },
		m_type{ NullMsgType{} },
		m_content{}
	{}

	message::message(MessageClass cls, msg_type type): message{ cls, type, detail::msg_content{} }
	{}

	message::message(MessageClass cls, msg_type type, detail::msg_content content):
		m_cls(cls),
		m_type(type),
		m_content(content)
	{}


	std::vector< char > encode_message(message const& msg)
	{
		YAML::Node encoding;

		auto cls = msg.get_class();
		encoding["class"] = cls;

		switch(cls)
		{
			case MessageClass::Peer2Peer:
			encoding["type"] = msg.get_type< MessageClass::Peer2Peer, Peer2PeerMsg >();
			break;

			case MessageClass::Peer2Client:
			encoding["type"] = msg.get_type< MessageClass::Peer2Client, Peer2ClientMsg >();
			break;

			case MessageClass::Client2Peer:
			encoding["type"] = msg.get_type< MessageClass::Client2Peer, Client2PeerMsg >();
			break;
		}

		encoding["content"] = msg.get_content().get();

		auto as_string = YAML::Dump(encoding);
		auto const msg_length = as_string.length();	// TODO: Check this is correct regardless of character encoding
		auto as_bytes = std::vector < char > {};
		as_bytes.resize(MessageSizeMarkerBytes + msg_length);
		*(size_t*)&as_bytes[0] = msg_length;
		std::copy(std::begin(as_string), std::end(as_string), as_bytes.begin() + MessageSizeMarkerBytes);
		return as_bytes;
	}

	message extract_message(std::string str)
	{
		auto nd = YAML::Load(str);
		auto cls = nd["class"].as< MessageClass >();
		auto content = detail::msg_content{ nd["content"] };
		switch(cls)
		{
			case MessageClass::Peer2Peer:
			return message{ cls, nd["type"].as< Peer2PeerMsg >(), content };

			case MessageClass::Peer2Client:
			return message{ cls, nd["type"].as< Peer2ClientMsg >(), content };

			case MessageClass::Client2Peer:
			return message{ cls, nd["type"].as< Client2PeerMsg >(), content };
		}
	}

}

