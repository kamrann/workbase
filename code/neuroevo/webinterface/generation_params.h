// generation_params.h

#ifndef __NE_GENERATION_PARAMS_H
#define __NE_GENERATION_PARAMS_H

#include "db_params.h"


class evorun_params_defn;
class genome_params_defn;

class generation_params_defn: public level_defn
{
public:
	typedef is_child_of< evorun_params_defn > parent_t;

	typedef mpl::vector<
		genome_params_defn
	>::type children_t;

	static const char* level_name()
	{
		return "Generation";
	}

	static param_spec_list get_specs()
	{
		param_spec_list psl;
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* */), "Generation #"));
//		double					genotype_diversity;		// TODO: type...?
		return psl;
	}
};



#endif



