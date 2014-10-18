// single_rigid_body.h

#ifndef __NE_RTP_PHYS2D_SINGLE_RIGID_BODY_H
#define __NE_RTP_PHYS2D_SINGLE_RIGID_BODY_H

#include "phys2d_object.h"


namespace sys {
	namespace phys2d {

		class single_rigid_body:
			public object
		{
		public:
			virtual void translate(b2Vec2 const& vec) override;
			virtual void rotate(float angle) override;
			virtual void set_linear_velocity(b2Vec2 const& linvel) override;
			virtual void set_angular_velocity(float angvel) override;

			virtual void set_position(b2Vec2 const& pos);
			virtual void set_orientation(float angle);
			virtual void apply_force(b2Vec2 const& force, b2Vec2 const& pos);
			virtual void apply_force_local(b2Vec2 const& lforce, b2Vec2 const& lpos);

			virtual b2Vec2 get_position() const override;
			virtual b2Vec2 get_linear_velocity() const override;
			virtual b2Vec2 get_local_linear_velocity() const;
			virtual float get_kinetic_energy() const override;

			virtual float get_orientation() const;
			virtual float get_angular_velocity() const;
			virtual float get_orientation_wrt_linear_velocity() const;

		protected:
			void set_body(b2Body* body);

		public:
			b2Body* m_body;
		};

	}
}



#endif

