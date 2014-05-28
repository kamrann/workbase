// phys_mlp_controller_params.h

#ifndef __NE_PHYS_MLP_CONTROLLER_PARAMS_H
#define __NE_PHYS_MLP_CONTROLLER_PARAMS_H

#include "db_params.h"


namespace phys {

	class phys_controller_params_defn;

	class mlp_controller_defn: public level_defn
	{
	public:
		typedef is_a< phys_controller_params_defn > parent_t;

//		typedef mpl::vector<

		static const char* level_name()
		{
			return "MLP Controller";
		}

		static param_spec_list get_specs()
		{
			param_spec_list psl;
			psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(), "Num Layers"));
			return psl;
		}

//		mlp_inputs_params	inputs;
	};

}


#endif



