// client_id.h
/*
Identification/address of client for peer use.
*/

#ifndef __WB_PWORK_CLIENT_ID_H
#define __WB_PWORK_CLIENT_ID_H

#include <boost/asio.hpp>


using boost::asio::ip::tcp;

namespace pwork {

	// TODO: unique identifier/uuid
	typedef unsigned long client_id;

	struct client_details
	{
		std::string name;
		tcp::endpoint endpt;
	};

}


#endif


