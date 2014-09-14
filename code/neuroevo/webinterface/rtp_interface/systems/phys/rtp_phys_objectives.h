// rtp_phys_objectives.h

#ifndef __NE_RTP_PHYS_OBJECTIVES_H
#define __NE_RTP_PHYS_OBJECTIVES_H

#include "rtp_phys_system.h"
#include "../../rtp_observer.h"

#include "ga/single_objective.h"


namespace rtp
{
	class agent_objective
	{
	public:
		enum Type {
			MaxFinalX,
			MaxAverageY,
			MinFinalLinearSpeed,
//			MaintainUprightStance,
			MinAverageKE,
			MaintainAltitude,
			MinNonFootContacts,

			Count,
		};

		static std::string const Names[Count];
		/*
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
		static i_phys_observer* create_instance(Type type, rtp_param param);
		*/
	};


	// TODO: May not need to distinguish objective from observer
	class phys_objective_fn: public i_observer
	{};

	template <
		typename ObjFn
	>
	class wrapped_objective_fn: public phys_objective_fn
	{
	public:
		typedef ObjFn obj_fn_t;

		typedef typename phys_system::agent_state agent_state_t;
		typedef typename phys_system::envt_state envt_state_t;
		typedef typename phys_system::trial_data trial_data_t;
		
		typedef single_objective< trial_data_t, obj_fn_t > resultant_obj_fn_t;
		typedef typename resultant_obj_fn_t::ofdata_t of_data_t;
		typedef typename resultant_obj_fn_t::obj_value_t obj_value_t;

	public:
		virtual void reset()
		{
			ofd = of_data_t();
		}

		virtual void update(i_system const* system)
		{
			ofd.update(system);// /* TODO: Decision ??? */(int)0, (agent_state_t const&)st, (envt_state_t const&)st);
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

