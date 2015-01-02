// sucker.h

#ifndef __NE_PHYS2D_SUCKER_H
#define __NE_PHYS2D_SUCKER_H

#include "system_sim/system_state_values.h"

#include <box2d/box2d.h>


namespace sys {
	namespace phys2d {

		class sucker
		{
		public:
			static std::shared_ptr< sucker > create(
				b2Body* body
				, b2Vec2 local_point
				//				, double max_force
				);

		public:
			inline bool is_attached() const
			{
				return joint_ != nullptr;
			}

			void attach(b2Body* target);
			void detach();

		public:	// should be private but can't work with make_shared
			sucker(b2Body* body, b2Vec2 local_point);

		protected:
			b2Body* body_;
			b2Vec2 local_point_;
			b2WeldJoint* joint_;
		};

	}
}


#endif

