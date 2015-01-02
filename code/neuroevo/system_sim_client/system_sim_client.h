// system_sim_client.h

#ifndef __WB_SYSSIM_CLIENT_H
#define __WB_SYSSIM_CLIENT_H

#include "ddl/ddl.h"
#include "system_sim/system_defn.h"


struct client_ddl
{
	ddl::defn_node get_defn(ddl::specifier& spc, std::map< std::string, std::shared_ptr< sys::i_system_defn > >& sys_defns);
};


#endif


