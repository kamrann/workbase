// phys_controller_params_tbl.h

#ifndef __GADB_PHYS_CONTROLLER_PARAMS_H
#define __GADB_PHYS_CONTROLLER_PARAMS_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_sys_params;
class phys_controller_type;

class phys_controller_params
{
public:
	dbo::ptr< phys_sys_params >	system;
	// TODO: Generic controller params?
	dbo::ptr< phys_controller_type > type;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, system, "system", dbo::NotNull | dbo::OnDeleteCascade);

		dbo::belongsTo(a, type, "phys_controller_type", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


