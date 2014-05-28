// evorun_params.h

#ifndef __NE_EVORUN_PARAMS_H
#define __NE_EVORUN_PARAMS_H

#include "db_params.h"


class system_params_defn;
class genalg_params_defn;
class generation_params_defn;

class evorun_params_defn: public level_defn
{
public:
//	typedef is_child_of< > parent_t;

	typedef mpl::vector<
		system_params_defn,
		genalg_params_defn
	>::type children_t;

	static const char* level_name()
	{
		return "Evo Run";
	}

	static param_spec_list get_specs()
	{
		param_spec_list psl;
		return psl;
	}
};



#endif



