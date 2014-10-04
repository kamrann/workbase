// nac_sys_params_tbl.h

#ifndef __GADB_NAC_SYS_PARAMS_H
#define __GADB_NAC_SYS_PARAMS_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class sys_params;
class sys_type;

class nac_sys_params
{
public:
	dbo::ptr< sys_params >	base_params;
//	dbo::ptr< sys_type >	type;	 TODO: how?
	//

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, base_params, "sys_params", dbo::NotNull | dbo::OnDeleteCascade);
		//dbo::field(a, initial_pop_size, "initial_pop_size");

//		dbo::belongsTo(a, base_params, "nac_sys_params__sys_params", );
//		dbo::belongsTo(a, type, "sys_params__sys_type");

//		dbo::hasMany(a, generations, dbo::ManyToOne, "gen_period");
	}
};


#endif


