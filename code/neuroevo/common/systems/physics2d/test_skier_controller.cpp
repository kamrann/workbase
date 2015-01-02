// test_skier_controller.cpp

#include "test_skier_controller.h"


namespace sys {
	namespace phys2d {

		std::string test_skier_controller_defn::get_name() const
		{
			return "test_skier_controller";
		}

		ddl::defn_node test_skier_controller_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("test_skier_controller")
				;
		}

		controller_ptr test_skier_controller_defn::create_controller(ddl::navigator nav) const
		{
			return std::make_unique< test_skier_controller >();
		}


		test_skier_controller::input_id_list_t test_skier_controller::get_input_ids() const
		{
			return{
				state_value_id::from_string("Time")
			};
		}

		test_skier_controller::output_list_t test_skier_controller::process(input_list_t const& inputs)
		{
			// 0 = shoulder torque, 1 = wrist torque
			auto const time = inputs[0];

			if(time < 3.0)
			{
				return{ 1., -1., 1.0, -1.0 };
			}
			else if(time < 5.0)
			{
				return{ 1., -1., 1.0, 1.0 };
			}
			else
			{
				return{ 1., -1., -1.0, 1.0 };
			}
		}

	}
}



