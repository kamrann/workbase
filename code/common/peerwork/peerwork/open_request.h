// open_request.h
/*
Information regarding any direct requests sent by a peer for which it is still awaiting the response.
*/

#ifndef __WB_PWORK_OPEN_REQUEST_H
#define __WB_PWORK_OPEN_REQUEST_H

#include "peer_id.h"


namespace pwork {

	struct open_request
	{
		peer_id pr;
		// TODO:
		size_t count;	// for now, this is number of jobs requested from peer

		open_request():
			pr{},
			count{ 0 }
		{}
	};

}


#endif


