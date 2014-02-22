// sat_shared_objective_fn_data_components.h

#ifndef __SAT_SHARED_OBJFN_DATA_COMPS_H
#define __SAT_SHARED_OBJFN_DATA_COMPS_H

#include "ga/objective_fn_data_component.h"


struct timesteps_ofd: public ofd_component
{
	size_t timesteps;

	timesteps_ofd(): timesteps(0)
	{}
	
	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		++ofd.timesteps;
	}
};

struct avg_linear_speed_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	double avg_lin_speed;

	avg_linear_speed_ofd(): avg_lin_speed(0.0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		ofd.avg_lin_speed += magnitude(agent_st.lin_velocity);
	}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{
		ofd.avg_lin_speed /= ofd.timesteps;
	}
};

struct avg_angular_speed_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	double avg_ang_speed;

	avg_angular_speed_ofd(): avg_ang_speed(0.0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		ofd.avg_ang_speed += magnitude(agent_st.ang_velocity);
	}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{
		ofd.avg_ang_speed /= ofd.timesteps;
	}
};

struct avg_orientation_delta_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	double avg_delta;

	avg_orientation_delta_ofd(): avg_delta(0.0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		// TODO:
		double const pi = boost::math::double_constants::pi;
		double const target = 0.75 * pi;
		double const delta = std::fmod(std::abs(agent_st.orientation - target), 2 * pi);
		ofd.avg_delta += delta > pi ? (2 * pi - delta) : delta;
	}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{
		ofd.avg_delta /= ofd.timesteps;
	}
};

struct stopped_rotating_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	bool stopped_rotating;
	size_t timesteps_to_stopped_rotating;

	stopped_rotating_ofd(): stopped_rotating(false), timesteps_to_stopped_rotating(0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		double const AcceptableSpeed = 0.025;	// TODO: With current impl, this needs to be dependent on maximum decel rate of ship...

		if(!ofd.stopped_rotating)
		{
			ofd.stopped_rotating = magnitude(agent_st.ang_velocity) <= AcceptableSpeed;
			ofd.timesteps_to_stopped_rotating = ofd.timesteps;
		}
	}
};

struct stopped_moving_linearly_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	bool stopped_moving;
	size_t timesteps_to_stopped_moving;

	stopped_moving_linearly_ofd(): stopped_moving(false), timesteps_to_stopped_moving(0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		double const AcceptableSpeed = 0.025;	// TODO: With current impl, this needs to be dependent on maximum decel rate of ship...

		if(!ofd.stopped_moving)
		{
			ofd.stopped_moving = magnitude(agent_st.lin_velocity) <= AcceptableSpeed;
			ofd.timesteps_to_stopped_moving = ofd.timesteps;
		}
	}
};

template < WorldDimensionality dim >
struct avg_resultant_force_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	double avg_resultant_force;

	avg_resultant_force_ofd(): avg_resultant_force(0.0)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		// TODO: Note that currently this does not take into account thruster power constant...
		typename DimensionalityTraits< dim >::force_t res_force;
		typename DimensionalityTraits< dim >::torque_t res_torque;
		boost::tie(res_force, res_torque) = agent_st.thruster_cfg->calc_resultants(dec, agent_st.c_of_mass);

		ofd.avg_resultant_force += magnitude(res_force);
	}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{
		ofd.avg_resultant_force /= ofd.timesteps;
	}
};

// Temp
#include "thrusters/thruster_presets.h"
//

struct opposing_thruster_activation_ofd: public ofd_component
{
	typedef mpl::vector< timesteps_ofd > dependencies;

	double opp_thrusters_freq;
	
	// Set of (non-zero) thruster activation subsets resulting in zero force and zero torque
	// TODO: Temp static 
	static std::set< thruster_base::thruster_activation > null_subsets;

	opposing_thruster_activation_ofd(): opp_thrusters_freq(0)
	{
		if(null_subsets.empty())
		{
			// TODO: Need more info. Maybe ofds should have some kind of init method which is passed scenario related data
			// Also, this is currently going to generate the set once for every single trial/agent, which is unnecessary...
			thruster_config< WorldDimensionality::dim2D > t_cfg = thruster_presets::square_minimal();
			thruster_config< WorldDimensionality::dim2D >::store_null_resultants ftr(
				t_cfg,
				Eigen::Vector2d(0, 0));
			thruster_config< WorldDimensionality::dim2D >::for_each_subset(t_cfg.num_thrusters(), ftr);
			null_subsets = ftr.null_sets;
		}
	}

//	template < typename agent_decision >
	bool activation_contains_null(thruster_base::thruster_activation const& ta) const
	{
		for(auto null_ta : null_subsets)
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

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		if(ofd.activation_contains_null(dec))
		{
			ofd.opp_thrusters_freq += 1.0;
		}
	}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{
		ofd.opp_thrusters_freq /= ofd.timesteps;
	}
};



#endif


