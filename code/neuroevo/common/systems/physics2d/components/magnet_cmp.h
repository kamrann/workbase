// magnet_cmp.h

#ifndef __NE_PHYS2D_MAGNET_CMP_H
#define __NE_PHYS2D_MAGNET_CMP_H

#include "../effectors/magnet.h"
#include "../sensors/basic_contact_sensor.h"


namespace sys {
	namespace phys2d {

		class magnet_cmp
		{
		public:
			static std::shared_ptr< magnet_cmp > create(
				b2Body* body
				, b2Vec2 local_point
				, double strength
				);

			static state_value_id_list get_state_values();

			size_t initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const;

		public:
			void apply_activation(double act);
			
			inline void update()
			{
				magnet_->update();
			}

		public:	// should be private but can't work with make_shared
			magnet_cmp(std::shared_ptr< magnet > mag);

		protected:
			std::shared_ptr< magnet > magnet_;
		};

	}
}


#endif

