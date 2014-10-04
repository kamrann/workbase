// message_tags.cpp

#include "message_tags.h"


namespace pwork {

	msg_tag_bm_t const tag_traits::mp = {
			{ MsgTag::ClientId, "client_id" },
			{ MsgTag::ClientName, "client_name" },
			{ MsgTag::ClientPrivateData, "client_private_data" },
			{ MsgTag::Count, "count" },
			{ MsgTag::JobDesc, "job_desc" },
			{ MsgTag::JobId, "job_id" },
			{ MsgTag::JobResult, "job_result" },
			{ MsgTag::JobUpdate, "job_update" },
			{ MsgTag::ListeningOn, "listening_on" },
			{ MsgTag::PeerId, "peer_id" },
			{ MsgTag::PeerList, "peer_list" },
			{ MsgTag::PeerName, "peer_name" },
	};

}




