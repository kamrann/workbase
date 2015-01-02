// component_object.h
/*
A physics entity (usually composite-body) composed of predefined components.
*/

#ifndef __NE_PHYS2D_COMPONENT_OBJECT_H
#define __NE_PHYS2D_COMPONENT_OBJECT_H

#include "component.h"

#include "b2d_script/parsing/load_script.h"


namespace sys {
	namespace phys2d {

		class component_object
		{
		public:
			static std::shared_ptr< component_object > create_from_script(b2ds::script const& scr, b2World* world);

		public:
			void add_component(std::shared_ptr< component > cmp, std::string name);
			void reset();

		public:
			state_value_id_list get_state_values() const;
			size_t initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const;
			size_t num_activations() const;
			void activate_effectors(effector_activations const& activations);

		protected:
			struct cmp_entry
			{
				std::string name;
				std::shared_ptr< component > cmp;
			};

			std::vector< cmp_entry > components_;

//			sensor_component_list sensors_;
//			effector_component_list effectors_;
		};

	}
}



#endif

