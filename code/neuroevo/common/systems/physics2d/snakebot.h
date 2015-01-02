// snakebot.h

#ifndef __NE_PHYS2D_SNAKEBOT_H
#define __NE_PHYS2D_SNAKEBOT_H

#include "composite_body.h"
#include "snakebot_defn.h"
#include "components/sucker_cmp.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class snakebot:
			public basic_agent,
			public composite_body
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

//			virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type) override;

		public:
			snakebot(
				std::shared_ptr< snakebot_defn::spec_data > _spec,
				std::shared_ptr< snakebot_defn::instance_data > _inst);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			std::shared_ptr< snakebot_defn::spec_data > m_spec;
			std::shared_ptr< snakebot_defn::instance_data > m_inst;

			std::vector< b2Body* > segments;
			std::vector< std::shared_ptr< sucker_cmp > > suckers;

			b2World* world;
		};

	}
}


#endif

