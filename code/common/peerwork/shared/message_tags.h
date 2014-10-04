// message_tags.h

#ifndef __WB_PWORK_MESSAGE_TAGS_H
#define __WB_PWORK_MESSAGE_TAGS_H

#include "job_id.h"
#include "job_description.h"
// TODO: Shared shouldn't really be dependent on non-shared!
#include "../peerwork/client_id.h"
#include "../peerwork/peer_id.h"

#include "util/bimap.h"


namespace pwork {

	enum class MsgTag {
		ClientId,				// Id of client with the peer, attached to all messages in both directions once established
		ClientName,
		ClientPrivateData,		// Unspecified data tag, not accessed by peer, but sometimes sent back in a response
		Count,
		JobDesc,
		JobId,
		JobResult,
		JobUpdate,
		ListeningOn,			// Sent from client to peer to indicate which port the client will be listening on
		PeerId,					// Identifies the source of a peer to peer 
		PeerList,
		PeerName,
	};

	typedef bimap< MsgTag, std::string > msg_tag_bm_t;
	
	struct tag_traits
	{
		typedef MsgTag type;
		static msg_tag_bm_t const mp;
	};


	template < MsgTag tag >
	struct tag_type_traits;

	template <>
	struct tag_type_traits < MsgTag::ClientId >
	{
		typedef client_id type;
	};

	template <>
	struct tag_type_traits < MsgTag::ClientName >
	{
		typedef std::string type;
	};

	template <>
	struct tag_type_traits < MsgTag::ClientPrivateData >
	{
		typedef std::string type;
	};

	template <>
	struct tag_type_traits < MsgTag::Count >
	{
		typedef int type;
	};

	template <>
	struct tag_type_traits < MsgTag::JobDesc >
	{
		typedef job_description type;
	};

	template <>
	struct tag_type_traits < MsgTag::JobId >
	{
		typedef job_id type;
	};

	template <>
	struct tag_type_traits < MsgTag::JobResult >
	{
		typedef std::string type;
	};

	template <>
	struct tag_type_traits < MsgTag::JobUpdate >
	{
		typedef std::string type;
	};

	template <>
	struct tag_type_traits < MsgTag::ListeningOn >
	{
		typedef int type;
	};

	template <>
	struct tag_type_traits < MsgTag::PeerId >
	{
		typedef peer_id type;
	};

	template <>
	struct tag_type_traits < MsgTag::PeerList >
	{
		typedef std::vector< peer_id_and_details > type;
	};

	template <>
	struct tag_type_traits < MsgTag::PeerName >
	{
		typedef std::string type;
	};

}



#endif


