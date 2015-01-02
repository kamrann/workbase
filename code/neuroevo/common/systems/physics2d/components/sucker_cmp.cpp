// sucker_cmp.cpp

#include "sucker_cmp.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< sucker_cmp > sucker_cmp::create(
			b2Body* body
			, b2Vec2 local_point
			, double range
//			, double max_force
			, b2dc::collision_filter sensor_filter
			)
		{
			auto impl = b2dc::sucker_cmp::create(body, local_point, range, sensor_filter);
			return std::make_shared< sucker_cmp >(impl);
		}

		state_value_id_list sucker_cmp::get_state_values() const
		{
			state_value_id_list svs;
			svs.push_back(state_value_id::from_string("attachable"));
			svs.push_back(state_value_id::from_string("attached"));
			svs.push_back(state_value_id::from_string("reaction_F"));
			return svs;
		}

		size_t sucker_cmp::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			auto svid = base + "attachable";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return impl_->can_attach();
			});

			svid = base + "attached";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return impl_->is_attached();
			});

			svid = base + "reaction_F";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return 0.0; // todo: joint_->GetReactionTorque(m_sys->get_hertz());
			});

			return bindings.size() - initial_count;
		}

		size_t sucker_cmp::num_activations() const
		{
			return 1;
		}

		void sucker_cmp::apply_activations(effector_activations const& act)
		{
			if(act[0] > 0.0)
			{
				impl_->attach();
			}
			else
			{
				impl_->detach();
			}
		}

		sucker_cmp::sucker_cmp(std::shared_ptr< b2dc::sucker_cmp > impl):
			impl_{ impl }
		{
			
		}

	}
}




