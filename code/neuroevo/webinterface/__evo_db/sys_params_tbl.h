// sys_params_tbl.h

#ifndef __GADB_SYS_PARAMS_H
#define __GADB_SYS_PARAMS_H

#include "table_traits.h"
//#include "sys_type_tbl.h"	// WTF dont get when ptr<> requires complete type?

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class evo_run;
class sys_type;

class sys_params
{
public:
	dbo::ptr< evo_run >	run;
//	TODO: Generic system params here
	dbo::ptr< sys_type > type;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, run, "evo_run", dbo::NotNull | dbo::OnDeleteCascade);
		//dbo::field(a, initial_pop_size, "initial_pop_size");

		dbo::belongsTo(a, type, "sys_params__sys_type", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


