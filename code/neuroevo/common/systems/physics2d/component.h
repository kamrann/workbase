// component.h

#ifndef __NE_PHYS2D_COMPONENT_H
#define __NE_PHYS2D_COMPONENT_H

#include "system_sim/system_state_values.h"
#include "system_sim/agent.h"


namespace sys {
	namespace phys2d {

		class effector_component
		{
		public:
			virtual size_t num_activations() const = 0;
			virtual void apply_activations(effector_activations const& act) = 0;
		};

		class sensor_component
		{
		public:
			virtual state_value_id_list get_state_values() const = 0;
			virtual size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const = 0;
		};

//		typedef std::vector< std::shared_ptr< effector_component > > effector_component_list;
//		typedef std::vector< std::shared_ptr< sensor_component > > sensor_component_list;

		class component:
			public effector_component
			, public sensor_component
		{};

		class sensor_only_component:
			public component
		{
		public:
			virtual size_t num_activations() const override
			{
				return 0;
			}

			virtual void apply_activations(effector_activations const& act) override
			{

			}
		};

	}
}



#endif

