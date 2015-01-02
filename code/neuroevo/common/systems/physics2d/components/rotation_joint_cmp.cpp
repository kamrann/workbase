// rotation_joint_cmp.cpp

#include "rotation_joint_cmp.h"


namespace sys {
	namespace phys2d {

/*		std::shared_ptr< rotation_joint_cmp > rotation_joint_cmp::create(
			b2Body* body
			, b2Vec2 local_point
			, double range
//			, double max_force
			, b2dc::collision_filter sensor_filter
			)
		{
			auto impl = b2dc::rotation_joint::create(body, local_point);
			return std::make_shared< rotation_joint_cmp >(impl);
		}
*/
		state_value_id_list rotation_joint_cmp::get_state_values() const
		{
			state_value_id_list svs;
			svs.push_back(state_value_id::from_string("angle"));
			svs.push_back(state_value_id::from_string("speed"));
			svs.push_back(state_value_id::from_string("applied"));
			svs.push_back(state_value_id::from_string("reaction_Fx"));
			svs.push_back(state_value_id::from_string("reaction_Fy"));
			svs.push_back(state_value_id::from_string("reaction_T"));
			return svs;
		}

		size_t rotation_joint_cmp::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			auto svid = base + "angle";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return impl_->get_angle();
			});

			svid = base + "speed";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return impl_->get_speed();
			});

			svid = base + "applied";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return impl_->get_applied_torque();
			});

			svid = base + "reaction_Fx";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return 0.0; //todo: joint_->GetReactionForce(m_sys->get_hertz()).x;
			});

			svid = base + "reaction_Fy";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return 0.0; //todo: joint_->GetReactionForce(m_sys->get_hertz()).y;
			});

			svid = base + "reaction_T";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return 0.0; //todo: joint_->GetReactionTorque(m_sys->get_hertz());
			});

			return bindings.size() - initial_count;
		}

		size_t rotation_joint_cmp::num_activations() const
		{
			return 1;
		}

		void rotation_joint_cmp::apply_activations(effector_activations const& act)
		{
			impl_->apply_activation(act[0]);
		}

		rotation_joint_cmp::rotation_joint_cmp(std::shared_ptr< b2dc::rotation_joint > impl):
			impl_{ impl }
		{
			
		}

	}
}




