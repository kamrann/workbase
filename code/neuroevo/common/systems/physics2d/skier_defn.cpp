// skier_defn.cpp

#include "skier_defn.h"
#include "skier.h"
#include "composite_body_defn.h"


namespace sys {
	namespace phys2d {

		std::string skier_defn::get_name() const
		{
			return "skier";
		}

		ddl::defn_node skier_defn::get_spec_defn(ddl::specifier& spc)
		{
			ddl::defn_node torso_width = spc.realnum("torso_width")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.3 })
				;

			ddl::defn_node torso_height = spc.realnum("torso_height")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.6 })
				;

			ddl::defn_node boot_width = spc.realnum("boot_width")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			ddl::defn_node boot_height = spc.realnum("boot_height")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			ddl::defn_node lower_leg_length = spc.realnum("lower_leg_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node lower_leg_thickness = spc.realnum("lower_leg_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			ddl::defn_node upper_leg_length = spc.realnum("upper_leg_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node upper_leg_thickness = spc.realnum("upper_leg_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.15 })
				;

			ddl::defn_node arm_length = spc.realnum("arm_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.75 })
				;

			ddl::defn_node arm_thickness = spc.realnum("arm_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.1 })
				;

			ddl::defn_node max_ankle_torque = spc.realnum("max_ankle_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node max_knee_torque = spc.realnum("max_knee_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node max_hip_torque = spc.realnum("max_hip_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node max_shoulder_torque = spc.realnum("max_shoulder_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node max_wrist_torque = spc.realnum("max_wrist_torque")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 })
				;

			ddl::defn_node ski_length = spc.realnum("ski_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 2.0 })
				;

			ddl::defn_node ski_thickness = spc.realnum("ski_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.05 })
				;

			ddl::defn_node ski_friction = spc.realnum("ski_friction")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.0 })
				;

			ddl::defn_node pole_length = spc.realnum("pole_length")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 })
				;

			ddl::defn_node pole_thickness = spc.realnum("pole_thickness")
				(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })
				(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.05 })
				;

			ddl::defn_node pole_friction = spc.realnum("pole_friction")
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
//				comp_defn.add_revolute("ankle");
				comp_defn.add_revolute("knee");
				comp_defn.add_revolute("hip");
				comp_defn.add_revolute("shoulder");
				comp_defn.add_revolute("wrist");
				auto composite_state_vals = comp_defn.get_state_values();
				svs.insert(std::end(svs), std::begin(composite_state_vals), std::end(composite_state_vals));

				svs.push_back(state_value_id::from_string("orientation"));

				auto contact_svs = contact_sensor_cmp::get_state_values();
				std::transform(
					std::begin(contact_svs),
					std::end(contact_svs),
					std::back_inserter(svs),
					[](state_value_id const& sv)
				{
					return state_value_id::from_string("ski_front") + sv;
				});
				std::transform(
					std::begin(contact_svs),
					std::end(contact_svs),
					std::back_inserter(svs),
					[](state_value_id const& sv)
				{
					return state_value_id::from_string("ski_back") + sv;
				});

				return svs;
			});

			num_effectors_fn_.set_fn([this](ddl::navigator nav)
			{
				// todo: not hard coded
				return 4;
			});

			return spc.composite("skier")(ddl::define_children{}
				("torso_width", torso_width)
				("torso_height", torso_height)
				("boot_width", boot_width)
				("boot_height", boot_height)
				("lower_leg_length", lower_leg_length)
				("lower_leg_thickness", lower_leg_thickness)
				("upper_leg_length", upper_leg_length)
				("upper_leg_thickness", upper_leg_thickness)
				("arm_length", arm_length)
				("arm_thickness", arm_thickness)
				("max_ankle_torque", max_ankle_torque)
				("max_knee_torque", max_knee_torque)
				("max_hip_torque", max_hip_torque)
				("max_shoulder_torque", max_shoulder_torque)
				("max_wrist_torque", max_wrist_torque)
				("ski_length", ski_length)
				("ski_thickness", ski_thickness)
				("ski_friction", ski_friction)
				("pole_length", pole_length)
				("pole_thickness", pole_thickness)
				("pole_friction", pole_friction)
				);
		}

		ddl::dep_function< state_value_id_list > skier_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list skier_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > skier_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > skier_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t skier_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr skier_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();

			spec->torso_width = spec_nav["torso_width"].get().as_real();
			spec->torso_height = spec_nav["torso_height"].get().as_real();
			spec->boot_width = spec_nav["boot_width"].get().as_real();
			spec->boot_height = spec_nav["boot_height"].get().as_real();
			spec->lower_leg_length = spec_nav["lower_leg_length"].get().as_real();
			spec->lower_leg_thickness = spec_nav["lower_leg_thickness"].get().as_real();
			spec->upper_leg_length = spec_nav["upper_leg_length"].get().as_real();
			spec->upper_leg_thickness = spec_nav["upper_leg_thickness"].get().as_real();
			spec->arm_length = spec_nav["arm_length"].get().as_real();
			spec->arm_thickness = spec_nav["arm_thickness"].get().as_real();
			spec->max_ankle_torque = spec_nav["max_ankle_torque"].get().as_real();
			spec->max_knee_torque = spec_nav["max_knee_torque"].get().as_real();
			spec->max_hip_torque = spec_nav["max_hip_torque"].get().as_real();
			spec->max_shoulder_torque = spec_nav["max_shoulder_torque"].get().as_real();
			spec->max_wrist_torque = spec_nav["max_wrist_torque"].get().as_real();
			spec->ski_length = spec_nav["ski_length"].get().as_real();
			spec->ski_thickness = spec_nav["ski_thickness"].get().as_real();
			spec->ski_friction = spec_nav["ski_friction"].get().as_real();
			spec->pole_length = spec_nav["pole_length"].get().as_real();
			spec->pole_thickness = spec_nav["pole_thickness"].get().as_real();
			spec->pole_friction = spec_nav["pole_friction"].get().as_real();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< skier >(spec, inst);
		}

	}
}


