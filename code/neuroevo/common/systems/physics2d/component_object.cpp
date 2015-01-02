// component_object.cpp
/*
A physics entity (usually composite-body) composed of predefined components.
*/

#include "component_object.h"
#include "components/rotation_joint_cmp.h"
#include "components/sucker_cmp.h"

#include "b2d_components/core/entity_data.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< component_object > component_object::create_from_script(b2ds::script const& scr, b2World* world)
		{
			b2ds::instantiation_data obj_data;
			if(!b2ds::instantiate_script(scr, world, &obj_data))
			{
				return nullptr;
			}

			auto obj = std::make_shared< component_object >();
			for(auto const& b : obj_data.bodies)
			{
				b2dc::entity_data ed{};
				ed.type = b2dc::entity_data::Type::Agent;
				// TODO: ed.type_value = static_cast<object*>(this);
				// TODO: ed.value = std::move(data);
				b2dc::set_body_data(b.second, std::move(ed));
			}
			for(auto const& h : obj_data.hinges)
			{
				obj->add_component(std::make_shared< rotation_joint_cmp >(h.second), h.first);
			}
			for(auto const& s : obj_data.suckers)
			{
				obj->add_component(std::make_shared< sucker_cmp >(s.second), s.first);
			}
			return obj;
		}

		void component_object::add_component(std::shared_ptr< component > cmp, std::string name)
		{
			components_.push_back({ name, cmp });
		}

		void component_object::reset()
		{
			components_.clear();
		}

		state_value_id_list component_object::get_state_values() const
		{
			state_value_id_list svs;
			for(auto const& ce : components_)
			{
				auto cmp_svs = ce.cmp->get_state_values();

				std::transform(
					std::begin(cmp_svs),
					std::end(cmp_svs),
					std::back_inserter(svs),
					[&ce](state_value_id const& svid)
				{
					return state_value_id::from_string(ce.name) + svid;
				});
			}
			return svs;
		}

		size_t component_object::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			for(auto const& ce : components_)
			{
				// todo: base should be passed into this method and prepended
				ce.cmp->initialize_state_value_bindings(bindings, accessors, state_value_id::from_string(ce.name));
			}

			return bindings.size() - initial_count;
		}

		size_t component_object::num_activations() const
		{
			size_t count = 0;
			for(auto const& ce : components_)
			{
				count += ce.cmp->num_activations();
			}
			return count;
		}
		
		void component_object::activate_effectors(effector_activations const& activations)
		{
			auto it = std::begin(activations);
			for(auto const& ce : components_)
			{
				auto count = ce.cmp->num_activations();
				auto it_end = it + count;
				ce.cmp->apply_activations(effector_activations{ it, it_end });
				it = it_end;
			}
		}

	}
}



