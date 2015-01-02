// passive_controller.cpp

#include "passive_controller.h"


namespace sys {

	std::string passive_controller_defn::get_name() const
	{
		return "passive";
	}

	ddl::defn_node passive_controller_defn::get_defn(ddl::specifier& spc)
	{
		return spc.composite("passive");
	}

	controller_ptr passive_controller_defn::create_controller(ddl::navigator nav) const
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


