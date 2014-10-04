// peer_state.h
/*
State information for a peer.
*/

#ifndef __WB_PWORK_PEER_STATE_H
#define __WB_PWORK_PEER_STATE_H


namespace pwork {

	struct peer_state
	{
		int jobs_available;

		peer_state():
			jobs_available{ 0 }
		{}
	};

}


#endif


