// sys_type_tbl.h

#ifndef __GADB_SYS_TYPE_H
#define __GADB_SYS_TYPE_H

#include "table_traits.h"

#include "../rtp_interface/systems/rtp_system.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class sys_params;

class sys_type
{
public:
	typedef dbo::collection< dbo::ptr< sys_params > >		sys_params_list;
	
	SystemType				type;
	std::string				name;
	sys_params_list			system_params;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, type, "type");
		dbo::field(a, name, "name");

		dbo::hasMany(a, system_params, dbo::ManyToOne, "sys_params__sys_type");
	}
};


#endif


