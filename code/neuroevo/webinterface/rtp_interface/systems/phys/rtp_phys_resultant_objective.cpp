// rtp_phys_resultant_objective.cpp

#include "rtp_phys_resultant_objective.h"
#include "../../rtp_single_objective.h"
#include "../../rtp_pareto.h"


namespace rtp_phys
{
	i_population_wide_observer* resultant_objective::create_instance(i_population_wide_observer::Type type, rtp_param param, std::set< agent_objective::Type >& required_observations)
	{
		switch(type)
		{
			case i_population_wide_observer::Single:
			{
				agent_objective::Type ao_type = boost::any_cast<agent_objective::Type>(param);
				required_observations.insert(ao_type);
				return new rtp_single_objective(agent_objective::Names[ao_type]);
			}

			case i_population_wide_observer::Pareto:
			{
				rtp_param_list param_list = boost::any_cast<rtp_param_list>(param);
				std::vector< std::string > obj_names;
				for(size_t i = 0; i < param_list.size(); ++i)
				{
					if(boost::any_cast<bool>(param_list[i]))
					{
						required_observations.insert((agent_objective::Type)i);
						obj_names.push_back(agent_objective::Names[i]);
					}
				}
				return new rtp_pareto(obj_names);
			}

			default:
			return nullptr;
		}
	}
}


