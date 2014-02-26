// rtp_sat_objectives.h

#ifndef __NE_RTP_SAT_OBJECTIVES_H
#define __NE_RTP_SAT_OBJECTIVES_H

#include "rtp_sat_system.h"
#include "rtp_sat_observers.h"

#include "ga/single_objective.h"


namespace rtp_sat
{
	// TODO: May not need to distinguish objective from observer
	template < WorldDimensionality dim >
	class objective_fn: public i_sat_observer< dim >
	{};

	template <
		WorldDimensionality dim,
		typename ObjFn
	>
	class wrapped_objective_fn: public objective_fn< dim >
	{
	public:
		typedef ObjFn obj_fn_t;

		typedef typename sat_system< dim >::agent_state agent_state_t;
		typedef typename sat_system< dim >::envt_state envt_state_t;
		typedef typename sat_system< dim >::trial_data trial_data_t;
		
		typedef single_objective< trial_data_t, obj_fn_t > resultant_obj_fn_t;
		typedef typename resultant_obj_fn_t::ofdata_t of_data_t;
		typedef typename resultant_obj_fn_t::obj_value_t obj_value_t;

	public:
		virtual void reset()
		{
			ofd = of_data_t();
		}

		virtual void update(/*decision,*/ state_t const& st)
		{
			ofd.update(/* TODO: Decision ??? */(int)0, (agent_state_t const&)st, (envt_state_t const&)st);
		}

		virtual boost::any/*obj_value_t*/ record_observations(trial_data_t const& td)
		{
			ofd.finalise();
			obj_value_t ov = resultant_obj_fn_t().evaluate(ofd, td);
			return boost::any(ov);
		}

	private:
		of_data_t ofd;
	};
}


#endif

