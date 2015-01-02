// magbot_defn.cpp

#include "magbot_defn.h"
#include "magbot.h"
#include "composite_body_defn.h"


namespace sys {
	namespace phys2d {

		std::string magbot_defn::get_name() const
		{
			return "magbot";
		}

		ddl::defn_node magbot_defn::get_spec_defn(ddl::specifier& spc)
		{
			ddl::defn_node torso_width = spc.realnum("torso_width")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node torso_height = spc.realnum("torso_height")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node arm_length = spc.realnum("arm_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node arm_thickness = spc.realnum("arm_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			ddl::defn_node max_arm_torque = spc.realnum("max_arm_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 2.0 })
				;

			ddl::defn_node magnet_strength = spc.realnum("magnet_strength")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			agent_state_values_fn_.set_fn([this](ddl::navigator nav)
			{
				auto svs = std::vector < state_value_id > {};
				state_value_id sv_id;

				// First, the state values inherited from composite_body
				auto object_state_vals = composite_body::get_state_values();
				svs.insert(std::end(svs), std::begin(object_state_vals), std::end(object_state_vals));

				composite_body_defn comp_defn;
				comp_defn.add_revolute("arm");
				auto composite_state_vals = comp_defn.get_state_values();
				svs.insert(std::end(svs), std::begin(composite_state_vals), std::end(composite_state_vals));

				auto magnet_svs = magnet_cmp::get_state_values();
				std::transform(
					std::begin(magnet_svs),
					std::end(magnet_svs),
					std::back_inserter(svs),
					[](state_value_id const& sv)
				{
					return state_value_id::from_string("magnet") + sv;
				});

				return svs;
			});

			num_effectors_fn_.set_fn([this](ddl::navigator nav)
			{
				// todo: not hard coded
				return 2;
			});

			return spc.composite("magbot")(ddl::define_children{}
				("torso_width", torso_width)
				("torso_height", torso_height)
				("arm_length", arm_length)
				("arm_thickness", arm_thickness)
				("max_arm_torque", max_arm_torque)
				("magnet_strength", magnet_strength)
				);
		}

		ddl::dep_function< state_value_id_list > magbot_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list magbot_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > magbot_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > magbot_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t magbot_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr magbot_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();

			spec->torso_width = spec_nav["torso_width"].get().as_real();
			spec->torso_height = spec_nav["torso_height"].get().as_real();
			spec->arm_length = spec_nav["arm_length"].get().as_real();
			spec->arm_thickness = spec_nav["arm_thickness"].get().as_real();
			spec->max_arm_torque = spec_nav["max_arm_torque"].get().as_real();
			spec->magnet_strength = spec_nav["magnet_strength"].get().as_real();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< magbot >(spec, inst);
		}

	}
}


