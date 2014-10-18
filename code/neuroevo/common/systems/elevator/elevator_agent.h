// elevator_agent.h

#ifndef __WB_NE_ELEVATOR_AGENT_H
#define __WB_NE_ELEVATOR_AGENT_H

#include "system_sim/agent.h"


namespace sys {
	namespace elev {

		class elevator_system;

		class elevator_agent:
			public i_agent
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

		public:
			elevator_agent();
			void set_system(elevator_system const* sys);

		private:
			elevator_system const* m_system;
		};

	}
}


#endif


