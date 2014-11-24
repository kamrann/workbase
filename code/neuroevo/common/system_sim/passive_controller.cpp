// passive_controller.cpp

#include "passive_controller.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sys {

	std::string passive_controller_defn::get_name() const
	{
		return "passive";
	}

	namespace sb = prm::schema;

	std::string passive_controller_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			return s;
		};

		return path.leaf().name();
	}

	controller_ptr passive_controller_defn::create_controller(prm::param_accessor acc) const
	{
		return std::make_unique< passive_controller >();
	}


	i_controller::input_id_list_t passive_controller::get_input_ids() const
	{
		return input_id_list_t();
	}

	i_controller::output_list_t passive_controller::process(input_list_t const& inputs)
	{
		return output_list_t();
	}

}


