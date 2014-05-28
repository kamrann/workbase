// rtp_sat_composite_observer.cpp

#include "rtp_sat_composite_observer.h"


namespace rtp_sat
{
	i_sat_observer* composite_observer::create_instance(std::set< agent_objective::Type > const& required_observations)
	{
		composite_observer* obs = new composite_observer();
		for(auto t : required_observations)
		{
			obs->m_single_value_observers[agent_objective::Names[t]] = agent_objective::create_instance(t, rtp_param());	// TODO: objective-specific params
		}
		return obs;
	}

	void composite_observer::reset()
	{
		for(auto obs : m_single_value_observers)
		{
			obs.second->reset();
		}
	}

	void composite_observer::update(/*decision,*/ state_t const& st)
	{
		for(auto obs : m_single_value_observers)
		{
			obs.second->update(st);
		}
	}

	i_observer::observations_t composite_observer::record_observations(trial_data_t const& td)
	{
		i_observer::observations_t observations;
		for(auto obs : m_single_value_observers)
		{
			// TODO: need separate interface for single observations
			i_observer::observations_t single = obs.second->record_observations(td);
			observations[obs.first] = single[""];
		}
		return observations;
	}
}



