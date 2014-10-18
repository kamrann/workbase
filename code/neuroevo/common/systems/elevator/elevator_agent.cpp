// elevator_agent.cpp

#include "elevator_agent.h"
#include "elevator_system.h"


namespace sys {
	namespace elev {

		std::string elevator_agent::get_name() const
		{
			return "The Elevator";
		}

		agent_sensor_list elevator_agent::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
/*			agent_sensor_list result;
			for(auto const& name : named_inputs)
			{
				result.emplace_back(m_system->m_state_value_ids.at(name));
			}
			return result;
*/
			return{};
		}

		double elevator_agent::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};// return m_system->m_state_value_accessors[sensor]();
		}

		void elevator_agent::activate_effectors(effector_activations const& activations)
		{

		}

		elevator_agent::elevator_agent():
			m_system(nullptr)
		{

		}

		void elevator_agent::set_system(elevator_system const* sys)
		{
			m_system = sys;
		}

	}
}



