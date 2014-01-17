// shared_objective_fn_data_components.h

#ifndef __SHARED_OBJFN_DATA_COMPS_H
#define __SHARED_OBJFN_DATA_COMPS_H

#include "objective_fn_data_component.h"


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


/*//////////////////////////////////////

struct A_: public ofd_base
{
	typedef mpl::vector0<> dependencies;
};

struct B_: public ofd_base
{
	typedef mpl::vector0<> dependencies;
};

struct F_: public ofd_base
{
	typedef mpl::vector< A_ > dependencies;
};

struct G_: public ofd_base
{
	typedef mpl::vector< B_ > dependencies;
};

struct H_: public ofd_base
{
	typedef mpl::vector< A_, B_ > dependencies;
};

struct P_: public ofd_base
{
	typedef mpl::vector< F_ > dependencies;
};

struct Q_: public ofd_base
{
	typedef mpl::vector< H_ > dependencies;
};

struct X_: public ofd_base
{
	typedef mpl::vector< H_, G_ > dependencies;
};

//////////////////////////////////////*/


#endif


