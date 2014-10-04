// peer_id.h
/*
Identification/address of a peer.
*/

#ifndef __WB_PWORK_PEER_ID_H
#define __WB_PWORK_PEER_ID_H

#include <boost/uuid/uuid.hpp>
#include <boost/asio.hpp>

#include <vector>


using boost::asio::ip::tcp;

namespace pwork {

	typedef boost::uuids::uuid peer_id;

	struct peer_details
	{
		std::string name;
		tcp::endpoint endpt;

		peer_details():
			name{},
			endpt{}
		{}

		peer_details(std::string _name, tcp::endpoint _endpt):
			name(_name),
			endpt(_endpt)
		{}
	};

	struct peer_id_and_details: public peer_details
	{
		peer_id id;

		peer_id_and_details():
			peer_details{},
			id{}
		{}

		peer_id_and_details(peer_id pid, peer_details details):
			peer_details{ details.name, details.endpt },
			id( pid )
		{
//			id = pid;
		}
	};

	typedef std::vector< peer_id > peer_id_list;
	typedef std::vector< peer_id_and_details > peer_list;

}


#endif


