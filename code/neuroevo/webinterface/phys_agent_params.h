// phys_agent_params.h

#ifndef __NE_PHYS_AGENT_PARAMS_H
#define __NE_PHYS_AGENT_PARAMS_H

#include "db_params.h"


namespace phys {

	class phys_system_params_defn;
	class spaceship_defn;
	
	class phys_agent_params_defn: public level_defn
	{
	public:
		typedef is_child_of< phys_system_params_defn > parent_t;

		typedef mpl::vector<
			spaceship_defn
		>::type subtypes_t;

		static const char* level_name()
		{
			return "Physics Agent";
		}

		static const char* subtype_name()
		{
			return "Physics Agent Type";
		}

		static param_spec_list get_specs()
		{
			param_spec_list psl;
			psl.push_back(param_spec(ParamType::Vector2, prm::vector2_par_constraints(), "Initial Position"));
			psl.push_back(param_spec(ParamType::RealNumber, prm::realnum_par_constraints(), "Initial Orientation"));
			psl.push_back(param_spec(ParamType::Vector2, prm::vector2_par_constraints(), "Initial Velocity"));
			psl.push_back(param_spec(ParamType::RealNumber, prm::realnum_par_constraints(), "Initial Angular Velocity"));
			return psl;
		}
	};
}




#endif



