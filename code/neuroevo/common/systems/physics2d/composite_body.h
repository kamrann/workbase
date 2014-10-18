// composite_body.h

#ifndef __NE_RTP_PHYS2D_COMPOSITE_BODY_H
#define __NE_RTP_PHYS2D_COMPOSITE_BODY_H

#include "phys2d_object.h"

#include <list>

#include <boost/any.hpp>


namespace sys {
	namespace phys2d {

		class composite_body:
			public object
		{
		public:
			virtual void translate(b2Vec2 const& vec) override;
			virtual void rotate(float angle) override;
			virtual void set_linear_velocity(b2Vec2 const& vel) override;
			virtual void set_angular_velocity(float vel) override;

			virtual b2Vec2 get_position() const override;
			virtual b2Vec2 get_linear_velocity() const override;
			virtual float get_kinetic_energy() const override;

			virtual b2Vec2 get_com_position() const;
			virtual b2Vec2 get_com_linear_velocity() const;

			static state_value_id_list get_state_values();
			size_t initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors) const;

		protected:
			void add_component_body(b2Body* comp, boost::any&& data);

		protected:
			std::list< b2Body* > m_bodies;
		};

	}
}



#endif

