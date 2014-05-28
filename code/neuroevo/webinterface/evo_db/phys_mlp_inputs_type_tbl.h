// phys_mlp_inputs_type_tbl.h

#ifndef __GADB_PHYS_MLP_INPUTS_TYPE_H
#define __GADB_PHYS_MLP_INPUTS_TYPE_H

#include "table_traits.h"

#include "../rtp_interface/systems/phys/controllers/mlp.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_mlp_controller_params;

class phys_mlp_inputs_type
{
public:
	typedef dbo::collection< dbo::ptr< phys_mlp_controller_params > >		phys_mlp_controller_params_list;
	
	rtp_phys::mlp_controller::Type			type;
	std::string								name;
	phys_mlp_controller_params_list			mlp_controller_params;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, type, "type");
		dbo::field(a, name, "name");

		dbo::hasMany(a, mlp_controller_params, dbo::ManyToOne, "phys_mlp_inputs_type");
	}
};


#endif


