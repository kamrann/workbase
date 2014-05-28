// animal.cpp

#include "params.h"


param_spec_list Animal::get_specs()
{
	param_spec_list sl;
	// TODO: Could define overloaded/template function so that ParamType is deduced from the member's actual type.
	sl.push_back(param_spec(ParamType::String, prm::string_par_constraints(), "Name"));
	sl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(), "Age"));
	param_spec ps_child(ParamType::ChildType);
/*	ps_child.child = new House();
	ps_child.name = ps_child.child->hierarchy_level_name();
	sl.push_back(ps_child);
*/	return sl;
}
