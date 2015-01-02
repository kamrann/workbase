// magbot.h

#ifndef __NE_PHYS2D_MAGBOT_H
#define __NE_PHYS2D_MAGBOT_H

#include "composite_body.h"
#include "magbot_defn.h"
#include "components/magnet_cmp.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class magbot:
			public basic_agent,
			public composite_body
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

		public:
			magbot(
				std::shared_ptr< magbot_defn::spec_data > _spec,
				std::shared_ptr< magbot_defn::instance_data > _inst);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			std::shared_ptr< magbot_defn::spec_data > m_spec;
			std::shared_ptr< magbot_defn::instance_data > m_inst;

			b2Body* body;
			b2Body* arm;

			std::shared_ptr< magnet_cmp > magnet;
		};

	}
}


#endif

