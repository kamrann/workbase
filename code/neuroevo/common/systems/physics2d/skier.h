// skier.h

#ifndef __NE_PHYS2D_SKIER_H
#define __NE_PHYS2D_SKIER_H

#include "composite_body.h"
#include "skier_defn.h"
#include "components/contact_sensor_cmp.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class skier:
			public basic_agent,
			public composite_body
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

		public:
			skier(
				std::shared_ptr< skier_defn::spec_data > _spec,
				std::shared_ptr< skier_defn::instance_data > _inst);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			std::shared_ptr< skier_defn::spec_data > m_spec;
			std::shared_ptr< skier_defn::instance_data > m_inst;

			b2Body* ski_and_boot;
//			b2Body* lower_leg;
			b2Body* upper_leg;
			b2Body* torso;
			b2Body* arm;
			b2Body* pole;

			std::shared_ptr< contact_sensor_cmp > ski_front_sensor, ski_back_sensor;
		};

	}
}


#endif

