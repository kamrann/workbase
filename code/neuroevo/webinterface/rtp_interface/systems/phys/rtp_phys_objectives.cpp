// rtp_phys_objectives.cpp

#include "rtp_phys_objectives.h"

#include "objectives/max_final_x_objective.h"
#include "objectives/max_average_y_objective.h"
#include "objectives/maintain_upright_stance_objective.h"

#include "../../params/boolean_par.h"


namespace rtp_phys
{
	std::string const agent_objective::Names[] = {
		"Max Final X",
		"Max Average Y",
		"Maintain Upright Stance",
	};

	agent_objective::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	size_t agent_objective::multi_param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return agent_objective::Type::Count;
	}

	rtp_named_param agent_objective::multi_param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		return rtp_named_param(new rtp_boolean_param_type(false), agent_objective::Names[index]);
	}

	/*	rtp_param_type* agent_objective::params(Type type)
	{
	switch(type)
	{
	// TODO:
	default:
	return new rtp_staticparamlist_param_type(rtp_named_param_list());
	}
	}
	*/
	i_phys_observer* agent_objective::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case MaxFinalX:				return new rtp_phys::wrapped_objective_fn< max_final_x_obj_fn >();
			case MaxAverageY:			return new rtp_phys::wrapped_objective_fn< max_average_y_obj_fn >();
			case MaintainUprightStance:	return new rtp_phys::wrapped_objective_fn< maintain_upright_stance_obj_fn >();

			default:
			assert(false);
			return nullptr;
		}
	}
}



