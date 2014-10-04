// discover.h

#ifndef __WB_PWORK_DISCOVER_H
#define __WB_PWORK_DISCOVER_H

#include <list>
#include <string>


namespace pwork {

	struct peer_identifier
	{
		std::string idstr;
	};


	typedef std::list< peer_identifier > peer_list;


	peer_list find_peers();

}


#endif


