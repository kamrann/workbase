// generic_agent.h

#ifndef __NE_PHYS2D_GENERIC_AGENT_H
#define __NE_PHYS2D_GENERIC_AGENT_H

#include "component.h"
#include "component_object.h"
#include "generic_agent_defn.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class generic_agent:
			public basic_agent
			//, public component_object
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

		public:
			generic_agent(
				std::shared_ptr< generic_agent_defn::spec_data > _spec,
				std::shared_ptr< generic_agent_defn::instance_data > _inst
				);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			std::shared_ptr< generic_agent_defn::spec_data > m_spec;
			std::shared_ptr< generic_agent_defn::instance_data > m_inst;

			std::shared_ptr< component_object > cmp_obj_;
		};

	}
}


#endif

