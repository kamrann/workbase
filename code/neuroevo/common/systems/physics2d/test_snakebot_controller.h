// test_snakebot_controller.h

#ifndef __NE_TEST_SNAKEBOT_CONTROLLER_H
#define __NE_TEST_SNAKEBOT_CONTROLLER_H

#include "system_sim/controller_defn.h"
#include "system_sim/controller.h"


namespace sys {
	namespace phys2d {

		class test_snakebot_controller_defn:
			public i_controller_defn
		{
		public:
			virtual std::string get_name() const override;
			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

			virtual controller_ptr create_controller(ddl::navigator nav) const override;
		};

		class test_snakebot_controller:
			public i_controller
		{
		public:
			virtual input_id_list_t get_input_ids() const override;
			virtual output_list_t process(input_list_t const& inputs) override;
		};

	}
}


#endif

