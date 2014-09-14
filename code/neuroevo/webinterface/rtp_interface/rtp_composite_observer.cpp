// rtp_composite_observer.cpp

#include "rtp_composite_observer.h"


namespace rtp
{
	std::unique_ptr< i_observer > composite_observer::create_instance(
		std::vector< std::unique_ptr< i_observer > > required_observations)
	{
		auto obs = std::make_unique< composite_observer >();
		size_t index = 0;
		for(auto& t : required_observations)
		{
			obs->m_single_value_observers[std::to_string(index)] = std::move(t);
			++index;
		}
		return std::move(obs);
	}

	void composite_observer::reset()
	{
		for(auto& obs : m_single_value_observers)
		{
			obs.second->reset();
		}
	}

	void composite_observer::update(i_system const* system)
	{
		for(auto& obs : m_single_value_observers)
		{
			obs.second->update(system);
		}
	}

	i_observer::observations_t composite_observer::record_observations(trial_data_t const& td)
	{
		i_observer::observations_t observations;
		for(auto const& obs : m_single_value_observers)
		{
			// TODO: need separate interface for single observations
			i_observer::observations_t single = obs.second->record_observations(td);
			auto it = *single.begin();
			observations[it.first] = it.second;
		}
		return observations;
	}
}



