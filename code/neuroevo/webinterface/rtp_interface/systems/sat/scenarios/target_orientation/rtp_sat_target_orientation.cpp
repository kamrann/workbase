// rtp_sat_target_orientation.cpp

#include "rtp_sat_target_orientation.h"

#include "thrusters/thruster_presets.h"


namespace rtp_sat {

	template < WorldDimensionality dim >
	rtp_named_param_list target_orientation< dim >::params()
	{
		rtp_named_param_list p;
		// TODO: dimensionality
		p.push_back(rtp_named_param(new rtp_integer_param_type(0, 0, 359), "Target"));
		return p;
	}

	template < WorldDimensionality dim >
	target_orientation< dim >::target_orientation(rtp_param param)
	{
		rtp_param_list& param_list = boost::any_cast<rtp_param_list&>(param);
		// TODO: Orientation type
		int target = boost::any_cast<int>(param_list[0]);
		m_target = (double)target;
	}

	template < WorldDimensionality dim >
	typename sat_system< dim >::state target_orientation< dim >::generate_initial_state(rgen_t& rgen)
	{
		typedef DimensionalityTraits< dim > dim_traits_t;

		state_t st;
		st.ship.thruster_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(thruster_presets::square_minimal()));
		st.thrusters.cfg = st.ship.thruster_cfg;
		st.thrusters.sync_to_cfg();

		double const InitialAngularSpeed = 0.5;

		// TODO: Random orientation routine for arbitrary dimensions
		st.ship.orientation = boost::random::uniform_01<>()(rgen)* 2 * boost::math::double_constants::pi - boost::math::double_constants::pi;
		st.ship.ang_velocity = random_dir_val< typename dim_traits_t::angular_velocity_t >(rgen) * InitialAngularSpeed;

		return st;
	}

	template < WorldDimensionality dim >
	typename target_orientation< dim >::scenario_data_t target_orientation< dim >::get_scenario_data() const
	{
		return scenario_data_t(m_target);
	}

}


