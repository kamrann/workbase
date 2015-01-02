// magnet_cmp.cpp

#include "magnet_cmp.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< magnet_cmp > magnet_cmp::create(
			b2Body* body
			, b2Vec2 local_point
			, double strength
			)
		{
			auto mag = magnet::create(body, local_point, strength);

			return std::make_shared< magnet_cmp >(mag);
		}

		state_value_id_list magnet_cmp::get_state_values()
		{
			state_value_id_list svs;
			svs.push_back(state_value_id::from_string("activated"));
			return svs;
		}

		void magnet_cmp::apply_activation(double act)
		{
			if(act > 0 && !magnet_->is_activated())
			{
				magnet_->switch_on();
			}
			else if(act <= 0 && magnet_->is_activated())
			{
				magnet_->switch_off();
			}
		}

		magnet_cmp::magnet_cmp(std::shared_ptr< magnet > mag):
			magnet_{ mag }
		{
			
		}

		size_t magnet_cmp::initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			auto svid = base + "activated";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return magnet_->is_activated();
			});

			return bindings.size() - initial_count;
		}

	}
}




