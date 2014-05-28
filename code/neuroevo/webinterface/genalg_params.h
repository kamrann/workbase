// genalg_params.h

#ifndef __NE_GENALG_PARAMS_H
#define __NE_GENALG_PARAMS_H

#include "db_params.h"


class evorun_params_defn;

class genalg_params_defn: public level_defn
{
public:
	typedef is_child_of< evorun_params_defn > parent_t;

	static const char* level_name()
	{
		return "Gen Alg";
	}

	static param_spec_list get_specs()
	{
		param_spec_list psl;
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* def 10, min 1 */), "Population Size"));
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* def 10, min 1 */), "Num Generations"));
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* def 1, min 1 */), "Trials/Generation"));
		psl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(/* def 0, min 0 */), "Random Seed"));
// TODO:		psl.push_back(param_spec(ParamType::Enumeration, prm::enum_par_constraints(/* def 10, min 1 */), "Procreation"));
		return psl;
	}
};



#endif



