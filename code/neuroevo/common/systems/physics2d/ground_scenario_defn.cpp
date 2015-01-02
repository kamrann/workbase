// ground_scenario_defn.cpp

#include "ground_scenario_defn.h"
#include "ground_scenario.h"


namespace sys {
	namespace phys2d {

		std::string ground_scenario_defn::get_name() const
		{
			return "ground_based";
		}

		ddl::defn_node ground_scenario_defn::get_defn(ddl::specifier& spc)
		{
			return spc.composite("ground_scenario")(ddl::define_children{}
				("ground_expanse", spc.realnum("ground_expanse")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0. })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 10. }))
				("incline", spc.realnum("incline")(ddl::spc_range < ddl::realnum_defn_node::value_t > { -90.0, 90.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.0 }))
				("friction", spc.realnum("friction")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0.0 })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 0.5 }))
				("gravity", spc.realnum("gravity")(ddl::spc_min < ddl::realnum_defn_node::value_t > { 0. })(ddl::spc_default < ddl::realnum_defn_node::value_t > { 9.81 }))
				("ball_count", spc.integer("ball_count")(ddl::spc_min < ddl::integer_defn_node::value_t > { 0 })(ddl::spc_default < ddl::integer_defn_node::value_t > { 0 }))
				);
		}

		state_value_id_list ground_scenario_defn::get_state_values(ddl::navigator nav) const
		{
			return{};
		}

		std::unique_ptr< scenario > ground_scenario_defn::create_scenario(ddl::navigator nav) const
		{
			spec_data spec;
			spec.expanse = nav["ground_expanse"].get().as_real();
			spec.incline = nav["incline"].get().as_real() * b2_pi / 180.0;
			spec.friction = nav["friction"].get().as_real();
			spec.gravity = nav["gravity"].get().as_real();
			spec.ball_count = nav["ball_count"].get().as_int();
			return std::make_unique< ground_scenario >(spec);
		}

	}
}


