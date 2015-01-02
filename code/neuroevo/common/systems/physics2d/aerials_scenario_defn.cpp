// aerials_scenario_defn.cpp

#include "aerials_scenario_defn.h"
#include "aerials_scenario.h"


namespace sys {
	namespace phys2d {

		std::string aerials_scenario_defn::get_name() const
		{
			return "aerials";
		}

		ddl::defn_node aerials_scenario_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("aerials_scenario")(ddl::define_children{}
				("gravity", spc.realnum("gravity")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 9.81 }))
				("surface_friction", spc.realnum("surface_friction")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.2 }))

				("overall_scale", spc.realnum("overall_scale")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.0 }))
				("platform_width", spc.realnum("platform_width")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 5.0 }))
				("slope_gradient", spc.realnum("slope_gradient")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.2 }))
				("slope_drop", spc.realnum("slope_drop")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 5.0 }))
				("dip_depth", spc.realnum("dip_depth")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 3.0 }))
				("ramp_gradient", spc.realnum("ramp_gradient")(ddl::spc_default < ddl::realnum_defn_node::value_t > { 1.5 }))
				("ramp_width", spc.realnum("ramp_width")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10.0 }))
				("landing_slope_drop", spc.realnum("landing_slope_drop")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10.0 }))
				("landing_flat_width", spc.realnum("landing_flat_width")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 5.0 }))
				("wall_height", spc.realnum("wall_height")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 2.0 }))
				);
		}

		state_value_id_list aerials_scenario_defn::get_state_values(ddl::navigator nav) const
		{
			return{};
		}

		std::unique_ptr< scenario > aerials_scenario_defn::create_scenario(ddl::navigator nav) const
		{
			spec_data spec;
			spec.gravity = nav["gravity"].get().as_real();
			spec.surface_friction = nav["surface_friction"].get().as_real();

			spec.overall_scale = nav["overall_scale"].get().as_real();
			spec.platform_width = nav["platform_width"].get().as_real();
			spec.slope_gradient = nav["slope_gradient"].get().as_real();
			spec.slope_drop = nav["slope_drop"].get().as_real();
			spec.dip_depth = nav["dip_depth"].get().as_real();
			spec.ramp_gradient = nav["ramp_gradient"].get().as_real();
			spec.ramp_width = nav["ramp_width"].get().as_real();
			spec.landing_slope_drop = nav["landing_slope_drop"].get().as_real();
			spec.landing_flat_width = nav["landing_flat_width"].get().as_real();
			spec.wall_height = nav["wall_height"].get().as_real();
			return std::make_unique< aerials_scenario >(spec);
		}

	}
}


