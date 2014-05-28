// system_params.h

#ifndef __NE_SYSTEM_PARAMS_H
#define __NE_SYSTEM_PARAMS_H

#include "db_params.h"


class evorun_params_defn;
namespace phys {
	class phys_system_params_defn;
}

class system_params_defn: public level_defn
{
public:
	typedef is_child_of< evorun_params_defn > parent_t;

	typedef mpl::vector<
		phys::phys_system_params_defn
	>::type subtypes_t;

	static const char* level_name()
	{
		return "System";
	}

	static const char* subtype_name()
	{
		return "System Type";
	}

	static param_spec_list get_specs()
	{
		param_spec_list psl;
		// TODO: any generic params?
		return psl;
	}
};



#endif



