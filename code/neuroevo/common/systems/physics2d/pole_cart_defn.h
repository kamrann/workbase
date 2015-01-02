// pole_cart_defn.h

#ifndef __NE_PHYS2D_POLE_CART_DEFN_H
#define __NE_PHYS2D_POLE_CART_DEFN_H

#include "phys2d_agent_defn.h"

#include "util/bimap.h"

#include <set>


namespace sys {
	namespace phys2d {

		class pole_cart_defn:
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
			enum class Joint {
				Pole,

				Count,
			};

			enum class JointStateValue {
				Angle,
				Speed,

				Count,
			};

			struct spec_data
			{
				float cart_mass;
				float pole_mass;
				float pole_length;
				float max_force;
				float fail_angle;
				float fail_displacement;
			};

			typedef phys2d_agent_defn::instance_data instance_data;	// TODO: placeholder

			static auto all_joints() -> std::set < Joint >
			{
				return{
					Joint::Pole,
				};
			}

			static bimap< Joint, std::string > const s_joint_names;
			static bimap< JointStateValue, std::string > const s_joint_sv_names;

			ddl::dep_function< state_value_id_list > agent_state_values_fn_;
			ddl::dep_function< size_t > num_effectors_fn_;

			friend class pole_cart;
		};

	}
}


#endif

