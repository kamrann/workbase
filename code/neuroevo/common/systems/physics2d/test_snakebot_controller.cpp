// test_snakebot_controller.cpp

#include "test_snakebot_controller.h"


namespace sys {
	namespace phys2d {

		std::string test_snakebot_controller_defn::get_name() const
		{
			return "test_snakebot_controller";
		}

		ddl::defn_node test_snakebot_controller_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("test_snakebot_controller")
				;
		}

		controller_ptr test_snakebot_controller_defn::create_controller(ddl::navigator nav) const
		{
			return std::make_unique< test_snakebot_controller >();
		}


		test_snakebot_controller::input_id_list_t test_snakebot_controller::get_input_ids() const
		{
			return{
				state_value_id::from_string("Time")
			};
		}

		test_snakebot_controller::output_list_t test_snakebot_controller::process(input_list_t const& inputs)
		{
			auto const time = inputs[0];

			// TODO: hard coded hack!!!!!!!!!!!!!!!!!!!!!

			if(time < 8.0)
			{
				return{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0 };
			}
			else
			{
				return{ -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0 };
			}
		}

	}
}



