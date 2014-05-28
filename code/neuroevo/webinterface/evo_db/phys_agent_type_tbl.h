// phys_agent_type_tbl.h

#ifndef __GADB_PHYS_AGENT_TYPE_H
#define __GADB_PHYS_AGENT_TYPE_H

#include "table_traits.h"

#include "../rtp_interface/systems/phys/rtp_phys_agent_body_spec.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_agent_params;

class phys_agent_type
{
public:
	typedef dbo::collection< dbo::ptr< phys_agent_params > >		phys_agent_params_list;
	
	rtp_phys::agent_body_spec::Type		type;
	std::string							name;
	phys_agent_params_list				agent_params;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, type, "type");
		dbo::field(a, name, "name");

		dbo::hasMany(a, agent_params, dbo::ManyToOne, "phys_agent_type");
	}
};


#endif


