// basic_biped.h

#ifndef __NE_PHYS2D_BASIC_BIPED_H
#define __NE_PHYS2D_BASIC_BIPED_H

#include "composite_body.h"
#include "basic_biped_defn.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class basic_biped:
			public basic_agent,
			public composite_body
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			double get_force_strength(basic_biped_defn::Joint jnt, double desired);
			virtual void activate_effectors(effector_activations const& activations) override;

			virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type) override;

		public:
			basic_biped(
				std::shared_ptr< basic_biped_defn::spec_data > _spec,
				std::shared_ptr< basic_biped_defn::instance_data > _inst);//,
//				phys2d_system* system);
			//void set_system(phys2d_system const* sys);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			//phys2d_system const* m_system;

			std::shared_ptr< basic_biped_defn::spec_data > m_spec;
			std::shared_ptr< basic_biped_defn::instance_data > m_inst;

			b2Body* pelvis;
			b2Body* upper1;
			b2Body* upper2;
			b2Body* lower1;
			b2Body* lower2;

//			std::array< b2RevoluteJoint*, (int)basic_biped_defn::Joint::Count > joints;

			struct muscle_state_t
			{
				double energy;
				double applied_torque;

				muscle_state_t():
					energy(1.0),
					applied_torque(0.0)
				{}
			};

			std::map< basic_biped_defn::Joint, muscle_state_t > muscle_states;

			bool left_foot_contact;
			bool right_foot_contact;

			double damage;
			double life;
		};

	}
}


#endif

