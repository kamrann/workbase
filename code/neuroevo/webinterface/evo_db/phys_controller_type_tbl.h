// phys_controller_type_tbl.h

#ifndef __GADB_PHYS_CONTROLLER_TYPE_H
#define __GADB_PHYS_CONTROLLER_TYPE_H

#include "table_traits.h"

#include "../rtp_interface/systems/phys/rtp_phys_evolvable_controller.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_controller_params;

class phys_controller_type
{
public:
	typedef dbo::collection< dbo::ptr< phys_controller_params > >		phys_controller_params_list;
	
	rtp_phys::evolvable_controller::Type	type;
	std::string								name;
	phys_controller_params_list				controller_params;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, type, "type");
		dbo::field(a, name, "name");

		dbo::hasMany(a, controller_params, dbo::ManyToOne, "phys_controller_type");
	}
};


#endif


