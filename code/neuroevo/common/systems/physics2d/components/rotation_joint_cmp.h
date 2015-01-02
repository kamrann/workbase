// rotation_joint_cmp.h

#ifndef __NE_PHYS2D_ROTATION_JOINT_CMP_H
#define __NE_PHYS2D_ROTATION_JOINT_CMP_H

#include "../component.h"
#include "b2d_components/components/rotation_joint.h"


namespace sys {
	namespace phys2d {

		class rotation_joint_cmp:
			public component
		{
		public:
			virtual state_value_id_list get_state_values() const override;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const override;

			virtual size_t num_activations() const override;
			virtual void apply_activations(effector_activations const& act) override;

		public:	// should be private but can't work with make_shared
			rotation_joint_cmp(std::shared_ptr< b2dc::rotation_joint > impl);

		protected:
			std::shared_ptr< b2dc::rotation_joint > impl_;
		};

	}
}


#endif

