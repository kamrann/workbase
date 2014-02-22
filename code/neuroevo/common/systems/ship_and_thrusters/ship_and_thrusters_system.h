// ship_and_thrusters_system.h

#ifndef __SHIP_AND_THRUSTERS_SYSTEM_H
#define __SHIP_AND_THRUSTERS_SYSTEM_H

#include "util/dimensionality.h"
#include "thrusters/thruster.h"


template <
	WorldDimensionality Dim_
>
class ship_and_thrusters_system
{
public:
	static WorldDimensionality const Dim = Dim_;

	typedef DimensionalityTraits< Dim > dim_traits_t;

	enum {
		ThrusterPower = 250,	// in N^-3
	};

/*	struct solution_result
	{
		typename dim_traits_t::force_t		force;
		typename dim_traits_t::torque_t		torque;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};
	*/
	typedef thruster_base::thruster_activation	solution_result;

public:
/*	void register_solution_result(solution_result const& res);	// TODO: Parameter identifying solution instance.
		// Also, ideally it would be possible not only for multiple solution instances to partake in a system simulation,
		// but even multiple different types of solution (to different problems, eg. predator & prey, or one ship aiming to
		// reach a given location, another ship aiming to destroy the first).
	
	void update();	// TODO: Time parameter?
*/
	// TODO: Something along lines of get_state()
};


#endif


