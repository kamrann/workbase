// message_types.h

#ifndef __WB_PWORK_MESSAGE_TYPES_H
#define __WB_PWORK_MESSAGE_TYPES_H

#include "util/bimap.h"

#include <boost/variant.hpp>

#include <string>


namespace pwork {

	enum class MessageClass {
		Peer2Peer,
		Peer2Client,
		Client2Peer,

		Undefined,
	};

	struct msg_cls_traits
	{
		typedef MessageClass type;
		static bimap< type, std::string > const mp;
	};

	enum class Peer2PeerMsg {
		Ping,					// Peer A sends its peer list (including itself) to Peer B. Any peers previously
								// unknown to B will be appended to its list and pinged themselves from B.
		AvailableWork,			// Notify that one or more jobs available on the sending peer's job queue
		RequestJobs,			// Ask a peer to give us a particular number of jobs
		DelegateJob,			// Gives a job to a peer
		NoMoreJobs,				// Tell a peer that requested a job that we have no more to give
		JobCompleted,			// For a peer to notify the owning peer that it has completed a job
		CancelJob,				// Notify a peer that we are retracting/cancelling a job it was doing for us
		JobUpdate,				// Mid-processing job status update, sent to job owning peer
	};

	struct p2p_msg_traits
	{
		typedef Peer2PeerMsg type;
		static bimap< type, std::string > const mp;
	};

	enum class Peer2ClientMsg {
		RegistrationAccepted,	// Confirm registration and provide client id, which client must include in all future messages
		JobAccepted,			// Confirm acceptance of job, provide job identifier
		JobStatus,				// Provide status information on ongoing job
		JobCompleted,			// Notify of job completion
	};

	struct p2c_msg_traits
	{
		typedef Peer2ClientMsg type;
		static bimap< type, std::string > const mp;
	};

	enum class Client2PeerMsg {
		Register,			// Register as a client of the peer
		Post,				// Post a new job
		QueryStatus,		// Query existing job
		Cancel,				// Cancel existing job
	};

	struct c2p_msg_traits
	{
		typedef Client2PeerMsg type;
		static bimap< type, std::string > const mp;
	};

	struct NullMsgType {};

	typedef boost::variant <
		NullMsgType,
		Peer2PeerMsg,
		Peer2ClientMsg,
		Client2PeerMsg
	> msg_type;

}



#endif


