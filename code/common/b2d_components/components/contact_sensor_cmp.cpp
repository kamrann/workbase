// contact_sensor_cmp.cpp

#include "contact_sensor_cmp.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< contact_sensor_cmp > contact_sensor_cmp::create(
			b2Body* body
			, b2Shape* shape
			, collision_filter filter
			)
		{
			auto sen = basic_contact_sensor::create(shape, body, filter);

			return std::make_shared< contact_sensor_cmp >(sen);
		}

		state_value_id_list contact_sensor_cmp::get_state_values()
		{
			state_value_id_list svs;
			svs.push_back(state_value_id::from_string("contact"));
			return svs;
		}

		contact_sensor_cmp::contact_sensor_cmp(std::shared_ptr< basic_contact_sensor > sen):
			sensor_{ sen }
		{
			
		}

		size_t contact_sensor_cmp::initialize_state_value_bindings_(sv_bindings_t& bindings, sv_accessors_t& accessors, state_value_id const& base) const
		{
			auto initial_count = bindings.size();
			auto bound_id = accessors.size();

			auto svid = base + "contact";
			bindings[svid] = bound_id++;
			accessors.push_back([this]
			{
				return sensor_->is_contacting();
			});

			return bindings.size() - initial_count;
		}

	}
}




