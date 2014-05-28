// phys_sys_params_tbl.h

#ifndef __GADB_PHYS_SYS_PARAMS_H
#define __GADB_PHYS_SYS_PARAMS_H

#include "table_traits.h"

#include "../rtp_interface/systems/phys/rtp_phys_scenario.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class sys_params;
class sys_type;

class phys_sys_params
{
public:
	dbo::ptr< sys_params >	base_params;
//	dbo::ptr< sys_type >	type;	 TODO: how?
	rtp_phys::phys_scenario_base::Type	scenario_type;
	double								duration;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, base_params, "sys_params", dbo::NotNull | dbo::OnDeleteCascade);
		dbo::field(a, scenario_type, "scenario_type");
		dbo::field(a, duration, "duration");

//		dbo::belongsTo(a, base_params, "phys_sys_params__sys_params");
//		dbo::belongsTo(a, type, "sys_params__sys_type");
	}
};


#endif


