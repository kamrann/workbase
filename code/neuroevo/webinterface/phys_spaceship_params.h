// phys_spaceship_params.h

#ifndef __NE_PHYS_SPACESHIP_PARAMS_H
#define __NE_PHYS_SPACESHIP_PARAMS_H

#include "db_params.h"


namespace phys {

	class phys_agent_params_defn;

	class spaceship_defn: public level_defn
	{
	public:
		typedef is_a< phys_agent_params_defn > parent_t;

		static const char* level_name()
		{
			return "Spaceship";
		}

		static param_spec_list get_specs()
		{
			param_spec_list psl;
			psl.push_back(param_spec(ParamType::RealNumber, prm::realnum_par_constraints(), "Thruster Power"));
			return psl;
		}
	};

}


#endif



