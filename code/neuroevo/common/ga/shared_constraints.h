// shared_constraint.h

#ifndef __SHARED_CONSTRAINTS_H
#define __SHARED_CONSTRAINTS_H

#include "util/dimensionality.h"
#include "thrusters/thruster.h"
#include "hard_constraint.h"


/*
Constrains ship thrusters to never fire in such a way that any subset of the active thrusters completely cancel out each
other's forces and torques.
*/
template < WorldDimensionality dim >
class opposed_thrusters_constraint: public hard_constraint< thruster_base::thruster_activation >
{
private:
	typedef DimensionalityTraits< dim >	dim_traits;

	// Set of (non-zero) thruster activation subsets resulting in zero force and zero torque
	std::set< thruster_base::thruster_activation > null_subsets;

public:
	opposed_thrusters_constraint(thruster_config< dim > const& t_cfg, typename dim_traits::position_t const& c_of_mass)
	{
		thruster_config< dim >::store_null_resultants ftr(t_cfg, c_of_mass);
		thruster_config< dim >::for_each_subset(t_cfg.num_thrusters(), ftr);
		null_subsets = ftr.null_sets;
	}

private:
	bool activation_contains_null(thruster_base::thruster_activation const& ta) const
	{
		for(auto null_ta: null_subsets)
		{
			bool ok = false;
			for(size_t i = 0; i < ta.size(); ++i)
			{
				if(null_ta[i] && !ta[i])
				{
					ok = true;
					break;
				}
			}

			if(!ok)
			{
				// Null activation subset included in ta
				return true;
			}
		}

		return false;
	}

public:
	bool test_satisfied(thruster_base::thruster_activation const& ta) const
	{
		return !activation_contains_null(ta);
	}
};


/*
Constrains ship thrusters to never fire in such a way that a resultant force (as opposed to a torque) is created.
*/
template < WorldDimensionality dim >
class no_force_constraint: public hard_constraint< thruster_base::thruster_activation >
{
private:
	typedef DimensionalityTraits< dim >	dim_traits;

	thruster_config< dim > cfg;
	typename dim_traits::position_t const& c_of_mass;

public:
	no_force_constraint(thruster_config< dim > const& _cfg, typename dim_traits::position_t const& _c_of_mass):
		cfg(_cfg), c_of_mass(_c_of_mass)
	{}

public:
	bool test_satisfied(thruster_base::thruster_activation const& ta) const
	{
		typename dim_traits::force_t force;
		typename dim_traits::torque_t torque;
		boost::tie(force, torque) = cfg.calc_resultants(ta, c_of_mass);
		
		double const epsilon = 0.0;	// TODO: ?
		return magnitude(force) <= epsilon;
	}
};


#endif


