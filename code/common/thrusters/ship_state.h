// ship_state.h

#ifndef __SHIP_STATE_H
#define __SHIP_STATE_H

#include "util/dimensionality.h"
#include "thrusters/thruster.h"


template < WorldDimensionality dim >
class ship_state
{
public:
	typedef typename DimensionalityTraits< dim >::position_t			pos_t;
	typedef typename DimensionalityTraits< dim >::velocity_t			vel_t;
	typedef typename DimensionalityTraits< dim >::force_t				force_t;

	typedef typename DimensionalityTraits< dim >::orientation_t			orient_t;
	typedef typename DimensionalityTraits< dim >::angular_velocity_t	ang_vel_t;
	typedef typename DimensionalityTraits< dim >::torque_t				torque_t;

public:
	pos_t					position;
	vel_t					lin_velocity;

	orient_t				orientation;
	ang_vel_t				ang_velocity;

	//thruster_system< dim >	thruster_sys;
	boost::shared_ptr< thruster_config< dim > >	thruster_cfg;

	// TODO: store center or mass here, or in a referenced ship_config?
	// maybe have class ship_system, incorporating thruster_system and c_of_mass, but not state,
	// and this class stores reference.
	pos_t					c_of_mass;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
public:
	ship_state()://boost::shared_ptr< thruster_config< dim > > t_cfg = nullptr):
		position(zero_val< pos_t >()),
		lin_velocity(zero_val< vel_t >()),
		orientation(zero_val< orient_t >()),	// TODO: Quaternion????
		ang_velocity(zero_val< ang_vel_t >()),
		//thruster_sys(t_cfg),
		thruster_cfg(nullptr),
		c_of_mass(zero_val< pos_t >())
	{
		init();
	}

	void init()
	{
		
	}
/*
	force_t get_resultant_force() const
	{
		force_t f, torque_t t;
		boost::tie(f, t) = thruster_sys.calc_resultants(c_of_mass);
		return f;
	}

	torque_t get_resultant_torque() const
	{
		force_t f, torque_t t;
		boost::tie(f, t) = thruster_sys.calc_resultants(c_of_mass);
		return t;
	}

	std::pair< force_t, torque_t > get_resultants() const
	{
		return thruster_sys.calc_resultants(c_of_mass);
	}
	*/
};


#endif


