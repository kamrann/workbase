// snakebot_defn.h

#ifndef __NE_PHYS2D_SNAKEBOT_DEFN_H
#define __NE_PHYS2D_SNAKEBOT_DEFN_H

#include "phys2d_agent_defn.h"

#include "util/bimap.h"

#include <set>


namespace sys {
	namespace phys2d {

		class snakebot_defn:
			public phys2d_agent_defn
		{
		public:
			virtual std::string get_name() const override;
			virtual ddl::defn_node get_spec_defn(ddl::specifier& spc) override;

			virtual ddl::dep_function< state_value_id_list >
				get_agent_state_values_fn() const override;
			virtual state_value_id_list get_agent_state_values(ddl::navigator nav) const override;

			virtual std::vector< std::string > sensor_inputs(ddl::navigator nav) const override;

			virtual ddl::dep_function< size_t > num_effectors_fn() const override;
			virtual size_t num_effectors(ddl::navigator nav) const override;

			virtual agent_ptr create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const override;

		private:
			struct spec_data
			{
				double segment_size;
				size_t num_segments;
				double max_angle;
				double max_torque;
				double friction;

				std::string tracking;
			};

			typedef phys2d_agent_defn::instance_data instance_data;	// TODO: placeholder

			ddl::dep_function< state_value_id_list > agent_state_values_fn_;
			ddl::dep_function< size_t > num_effectors_fn_;

			friend class snakebot;
		};

	}
}


#endif

