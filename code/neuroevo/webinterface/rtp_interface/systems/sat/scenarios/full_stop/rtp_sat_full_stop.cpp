// rtp_sat_full_stop.cpp

#include "rtp_sat_full_stop.h"


namespace rtp_sat {

	template < WorldDimensionality dim >
	rtp_named_param_list full_stop< dim >::params()
	{
		rtp_named_param_list p;
		return p;
	}

	template < WorldDimensionality dim >
	full_stop< dim >::full_stop(rtp_param param)
	{
		rtp_param_list& param_list = boost::any_cast<rtp_param_list&>(param);

	}

	template < WorldDimensionality dim >
	typename sat_system< dim >::state full_stop< dim >::generate_initial_state(rgen_t& rgen) const
	{
		typedef DimensionalityTraits< dim > dim_traits_t;

		sat_system< dim >::state st;

		double const InitialLinearSpeed = 2.0;
		double const InitialAngularSpeed = 0.5;
		
		st.ship.lin_velocity = random_dir_val< typename dim_traits_t::velocity_t >(rgen) * InitialLinearSpeed;
		st.ship.ang_velocity = //random_dir_val< typename dim_traits_t::angular_velocity_t >(rgen) * InitialAngularSpeed;
			m_initial_ang_vel.get(rgen);
		
		return st;
	}

	template class full_stop< WorldDimensionality::dim2D >;
	//template class full_stop< WorldDimensionality::dim3D >;
}


