// sucker_cmp.h

#ifndef __NE_PHYS2D_SUCKER_CMP_H
#define __NE_PHYS2D_SUCKER_CMP_H

#include "../component.h"
#include "b2d_components/components/sucker_cmp.h"
//#include "../effectors/sucker.h"
//#include "../sensors/basic_contact_sensor.h"


namespace sys {
	namespace phys2d {

		class sucker_cmp:
			public component
		{
		public:
			static std::shared_ptr< sucker_cmp > create(
				b2Body* body
				, b2Vec2 local_point
				, double range
				//				, double max_force
				, b2dc::collision_filter sensor_filter
				);

			virtual state_value_id_list get_state_values() const override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const override;
			
			virtual size_t num_activations() const override;
			virtual void apply_activations(effector_activations const& act) override;

		public:	// should be private but can't work with make_shared
			sucker_cmp(std::shared_ptr< b2dc::sucker_cmp > impl);

		protected:
			std::shared_ptr< b2dc::sucker_cmp > impl_;
		};

	}
}


#endif

