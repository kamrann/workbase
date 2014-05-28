// rtp_sat_objectives.h

#ifndef __NE_RTP_SAT_OBJECTIVES_H
#define __NE_RTP_SAT_OBJECTIVES_H

#include "rtp_sat_system.h"
#include "rtp_sat_observers.h"

#include "ga/single_objective.h"


namespace rtp_sat
{
	class agent_objective
	{
	public:
		enum Type {
			MinAverageKE,

			Count,
		};

		static std::string const Names[Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		class multi_param_type: public rtp_paramlist_param_type
		{
		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;
		};

		//static rtp_param_type* params(Type type);
		static i_sat_observer* create_instance(Type type, rtp_param param);
	};


	// TODO: May not need to distinguish objective from observer
	template < WorldDimensionality dim >
	class sat_objective_fn: public i_sat_observer< dim >
	{};

	template <
		WorldDimensionality dim,
		typename ObjFn
	>
	class wrapped_objective_fn: public sat_objective_fn< dim >
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

		virtual observations_t record_observations(trial_data_t const& td)
		{
			ofd.finalise();
			obj_value_t ov = resultant_obj_fn_t().evaluate(ofd, td);
			observations_t res;
			res[""] = boost::any(ov);
			return res;
		}

	private:
		of_data_t ofd;
	};
}


#endif

