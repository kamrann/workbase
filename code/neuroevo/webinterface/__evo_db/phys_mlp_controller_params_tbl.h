// phys_mlp_controller_params_tbl.h

#ifndef __GADB_PHYS_MLP_CONTROLLER_PARAMS_H
#define __GADB_PHYS_MLP_CONTROLLER_PARAMS_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class phys_controller_params;
//class phys_controller_type;
class phys_mlp_inputs_type;

class phys_mlp_controller_params
{
public:
	dbo::ptr< phys_controller_params >	base;
	//dbo::ptr< phys_controller_type > type;
	int num_layers;
	dbo::ptr< phys_mlp_inputs_type >	mlp_type;

	template < class Action >
	void persist(Action& a)
	{
		dbo::id(a, base, "phys_controller_params", dbo::NotNull | dbo::OnDeleteCascade);
		dbo::field(a, num_layers, "num layers");

		dbo::belongsTo(a, mlp_type, "phys_mlp_inputs_type", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


