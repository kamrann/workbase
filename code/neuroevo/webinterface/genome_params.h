// genome_params.h

#ifndef __NE_GENOME_PARAMS_H
#define __NE_GENOME_PARAMS_H

#include "db_params.h"


class generation_params_defn;

class genome_params_defn: public level_defn
{
public:
	typedef is_child_of< generation_params_defn > parent_t;

	static const char* level_name()
	{
		return "Genome";
	}

	static param_spec_list get_specs()
	{
		param_spec_list psl;
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* */), "Objective Rank"));	// Rank (by objective function) of this individual within its generation
//		std::vector< unsigned char >	encoding;
		return psl;
	}
};



#endif



