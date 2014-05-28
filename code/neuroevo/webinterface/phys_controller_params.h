// phys_controller_params.h

#ifndef __NE_PHYS_CONTROLLER_PARAMS_H
#define __NE_PHYS_CONTROLLER_PARAMS_H

#include "db_params.h"


namespace phys {

	class phys_system_params_defn;
	class mlp_controller_defn;

	class phys_controller_params_defn: public level_defn
	{
	public:
		typedef is_child_of< phys_system_params_defn > parent_t;

		typedef mpl::vector<
			mlp_controller_defn
		>::type subtypes_t;

		static const char* level_name()
		{
			return "Physics Controller";
		}

		static const char* subtype_name()
		{
			return "Physics Controller Type";
		}

		static param_spec_list get_specs()
		{
			param_spec_list psl;
			// Generic?
			return psl;
		}
	};

}




#endif



