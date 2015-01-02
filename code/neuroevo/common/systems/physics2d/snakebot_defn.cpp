// snakebot_defn.cpp

#include "snakebot_defn.h"
#include "snakebot.h"
#include "composite_body_defn.h"


namespace sys {
	namespace phys2d {

		std::string snakebot_defn::get_name() const
		{
			return "snakebot";
		}

		ddl::defn_node snakebot_defn::get_spec_defn(ddl::specifier& spc)
		{
			ddl::defn_node segment_size = spc.realnum("segment_size")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node num_segments = spc.integer("num_segments")
				(ddl::spc_min < ddl::integer_defn_node::value_t > { 2 })
				(ddl::spc_default < ddl::integer_defn_node::value_t > { 4 })
				;

			ddl::defn_node max_angle = spc.realnum("max_angle")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 30 })
				;

			ddl::defn_node max_torque = spc.realnum("max_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10 })
				;

			ddl::defn_node friction = spc.realnum("friction")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			auto rf_num_segments = ddl::node_ref{ num_segments };
			agent_state_values_fn_.set_fn([this, rf_num_segments](ddl::navigator nav)
			{
				auto svs = std::vector < state_value_id > {};
				state_value_id sv_id;

				// First, the state values inherited from composite_body
				auto object_state_vals = composite_body::get_state_values();
				svs.insert(std::end(svs), std::begin(object_state_vals), std::end(object_state_vals));

				composite_body_defn comp_defn;
				auto num_segments = ddl::nav_to_ref(rf_num_segments, nav).get().as_int();
				for(size_t i = 0; i < num_segments - 1; ++i)
				{
					std::string jnt_name = "joint" + std::to_string(i + 1);
					comp_defn.add_revolute(jnt_name);
				}
				auto composite_state_vals = comp_defn.get_state_values();
				svs.insert(std::end(svs), std::begin(composite_state_vals), std::end(composite_state_vals));

//				svs.push_back(state_value_id::from_string("left_contact"));
//				svs.push_back(state_value_id::from_string("right_contact"));
				for(size_t i = 0; i < num_segments; ++i)
				{
					auto prefix = std::string{ "sucker" } +std::to_string(i + 1);
					auto sucker_svs = sucker_cmp::get_state_values();
					for(auto const& sv : sucker_svs)
					{
						svs.push_back(state_value_id::from_string(prefix) + sv);
					}
				}

				return svs;
			});
			agent_state_values_fn_.add_dependency(ddl::node_dependency(rf_num_segments));

			num_effectors_fn_.set_fn([this, rf_num_segments](ddl::navigator nav)
			{
				auto num_segments = ddl::nav_to_ref(rf_num_segments, nav).get().as_int();
				auto num_joints = num_segments - 1;
				auto num_suckers = num_segments;// 2;
				return num_joints + num_suckers;
			});
			num_effectors_fn_.add_dependency(ddl::node_dependency(rf_num_segments));

			return spc.composite("snakebot")(ddl::define_children{}
				("segment_size", segment_size)
				("num_segments", num_segments)
				("max_angle", max_angle)
				("max_torque", max_torque)
				("friction", friction)

				("tracking", spc.enumeration("tracking")
				(ddl::define_enum_fixed{}("torque")("speed")("position"))
				)
				);
		}

		ddl::dep_function< state_value_id_list > snakebot_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list snakebot_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > snakebot_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > snakebot_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t snakebot_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr snakebot_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();

			spec->segment_size = spec_nav["segment_size"].get().as_real();
			spec->num_segments = spec_nav["num_segments"].get().as_int();
			spec->max_angle = spec_nav["max_angle"].get().as_real() * b2_pi / 180.0;
			spec->max_torque = spec_nav["max_torque"].get().as_real();
			spec->friction = spec_nav["friction"].get().as_real();

			spec->tracking = spec_nav["tracking"].get().as_single_enum_str();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< snakebot >(spec, inst);
		}

	}
}


