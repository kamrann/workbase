// agent.h

#ifndef __NE_SYSSIM_AGENT_H
#define __NE_SYSSIM_AGENT_H

#include "system_state_values.h"

#include <string>
#include <vector>


namespace sys {

	typedef int agent_sensor_id;
	typedef std::vector< agent_sensor_id > agent_sensor_list;
	typedef std::vector< std::string > agent_sensor_name_list;	// Ordered - index corresponds to agent_sensor_id

	typedef std::vector< double > effector_activations;


	class i_agent
	{
	public:
		virtual std::string get_name() const = 0;

//		virtual state_value_bound_id get_state_value_binding(state_value_id svid) const = 0;
//		virtual double get_state_value(state_value_bound_id bid) const = 0;

		virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const = 0;
		virtual double get_sensor_value(agent_sensor_id const& sensor) const = 0;

		virtual void activate_effectors(effector_activations const& activations) = 0;

	public:
		virtual ~i_agent() {}
	};

}


#endif

