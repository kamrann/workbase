// ski_jump_scenario_defn.cpp

#include "ski_jump_scenario_defn.h"
#include "ski_jump_scenario.h"


namespace sys {
	namespace phys2d {

		std::string ski_jump_scenario_defn::get_name() const
		{
			return "ski_jump";
		}

		ddl::defn_node ski_jump_scenario_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("ski_jump_scenario")(ddl::define_children{}
				("gravity", spc.realnum("gravity")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 9.81 }))
				("surface_friction", spc.realnum("surface_friction")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.2 }))

				("overall_scale", spc.realnum("overall_scale")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 }))
				("platform_width", spc.realnum("platform_width")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 5.0 }))
				("inrun_incline", spc.realnum("inrun_incline")(ddl::spc_range < ddl::realnum_defn_node::value_t > { 0.0, 90.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 45.0 }))
				("inrun_length", spc.realnum("inrun_length")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10.0 }))
				("transition1_radius", spc.realnum("transition1_radius")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 5.0 }))
				("takeoff_incline", spc.realnum("takeoff_incline")(ddl::spc_range < ddl::realnum_defn_node::value_t > { -90.0, 90.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.0 }))
				("takeoff_length", spc.realnum("takeoff_length")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10.0 }))
				("jump_dropoff", spc.realnum("jump_dropoff")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 3.0 }))
				("landing_length", spc.realnum("landing_length")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 30.0 }))
				("landing_drop", spc.realnum("landing_drop")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 15.0 }))
				("landing_x1", spc.realnum("landing_x1")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 2.5 }))
				("landing_x2", spc.realnum("landing_x2")(ddl::spc_default < ddl::realnum_defn_node::value_t > { -6.0 }))
				("wall_height", spc.realnum("wall_height")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 2.0 }))
				);
		}

		state_value_id_list ski_jump_scenario_defn::get_state_values(ddl::navigator nav) const
		{
			return{
				state_value_id::from_string("jump_distance"),
				state_value_id::from_string("on_platform"),
				state_value_id::from_string("platform_pos"),
				state_value_id::from_string("on_takeoff"),
				state_value_id::from_string("takeoff_pos"),
			};
		}

		std::unique_ptr< scenario > ski_jump_scenario_defn::create_scenario(ddl::navigator nav) const
		{
			spec_data spec;
			spec.gravity = nav["gravity"].get().as_real();
			spec.surface_friction = nav["surface_friction"].get().as_real();

			spec.overall_scale = nav["overall_scale"].get().as_real();
			spec.platform_width = nav["platform_width"].get().as_real();
			spec.inrun_degrees = nav["inrun_incline"].get().as_real() * b2_pi / 180.0;
			spec.inrun_length = nav["inrun_length"].get().as_real();
			spec.transition1_radius = nav["transition1_radius"].get().as_real();
			spec.takeoff_degrees = nav["takeoff_incline"].get().as_real() * b2_pi / 180.0;
			spec.takeoff_length = nav["takeoff_length"].get().as_real();
			spec.jump_dropoff = nav["jump_dropoff"].get().as_real();
			spec.landing_length = nav["landing_length"].get().as_real();
			spec.landing_drop = nav["landing_drop"].get().as_real();
			spec.landing_x1 = nav["landing_x1"].get().as_real();
			spec.landing_x2 = nav["landing_x2"].get().as_real();
			spec.wall_height = nav["wall_height"].get().as_real();
			return std::make_unique< ski_jump_scenario >(spec);
		}

	}
}


