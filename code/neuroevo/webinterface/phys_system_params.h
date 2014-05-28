// phys_system_params.h

#ifndef __NE_PHYS_SYSTEM_PARAMS_H
#define __NE_PHYS_SYSTEM_PARAMS_H

#include "db_params.h"


class system_params_defn;

namespace phys {

	class phys_agent_params_defn;
	class phys_controller_params_defn;

	class phys_system_params_defn: public level_defn
	{
	public:
		typedef is_a< system_params_defn > parent_t;

		typedef mpl::vector<
			phys_agent_params_defn,
			phys_controller_params_defn
		>::type children_t;

		static const char* level_name()
		{
			return "2D Physics Sim";
		}

		static param_spec_list get_specs()
		{
			param_spec_list psl;
			// TODO: Scenario?
			psl.push_back(param_spec(ParamType::RealNumber, prm::realnum_par_constraints(), "Duration"));
			return psl;
		}
	};

}


#endif



