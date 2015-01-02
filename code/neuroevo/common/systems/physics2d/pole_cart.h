// pole_cart.h

#ifndef __NE_PHYS2D_POLE_CART_H
#define __NE_PHYS2D_POLE_CART_H

#include "composite_body.h"
#include "pole_cart_defn.h"

#include "system_sim/basic_agent.h"

#include <Box2D/Box2D.h>


namespace sys {
	namespace phys2d {

		class phys2d_system;

		class pole_cart:
			public basic_agent,
			public composite_body
			// TODO: really shouldn't derive from this, since doesn't have full freedoms of a normal body
		{
		public:
			virtual std::string get_name() const override;

			virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
			virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

			virtual void activate_effectors(effector_activations const& activations) override;

			virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type) override;

		public:
			pole_cart(
				std::shared_ptr< pole_cart_defn::spec_data > _spec,
				std::shared_ptr< pole_cart_defn::instance_data > _inst);
			//void set_system(phys2d_system const* sys);

		protected:
			virtual void create(i_system* sys) override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const override;

		private:
			//phys2d_system const* m_system;

			std::shared_ptr< pole_cart_defn::spec_data > m_spec;
			std::shared_ptr< pole_cart_defn::instance_data > m_inst;

			b2Body* track;	// TODO: ideally part of scenario environment
			b2Body* cart;
			b2Body* pole;
			b2RevoluteJoint* pole_joint;
			b2PrismaticJoint* track_joint;
		};

	}
}


#endif

