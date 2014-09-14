// rtp_sat_scenario.h

#ifndef __NE_RTP_SAT_SCENARIO_H
#define __NE_RTP_SAT_SCENARIO_H

#include "rtp_sat_system.h"
#include "../rtp_scenario.h"
#include "../../rtp_param.h"
#include "../../params/enum_par.h"
#include "../../params/paramlist_par.h"
#include "../../params/nestedparam_par.h"

#include "util/fixed_or_random.h"
#include "thrusters/thruster.h"

#include <array>


struct rtp_named_param_list;

namespace rtp_sat
{
	class sat_scenario_base	// ?? i_scenario
	{
	public:
		enum ScenarioType {
//			AngularFullStop,
//			LinearFullStop,
			FullStop,
			TargetOrientation,

			NumScenarios,
		};

		static const std::array< std::string, NumScenarios > ScenarioNames;
	};

	template < WorldDimensionality dim >
	class sat_scenario: public sat_scenario_base
	{
	public:
		static WorldDimensionality const Dim = dim;
		typedef DimensionalityTraits< Dim > dim_traits_t;

		// The following using Dim instead of dim generates errors. Need constexpr above?? 
		typedef sat_system< dim > system_t;
		typedef typename system_t::state state_t;
		typedef typename system_t::scenario_data scenario_data_t;

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		class param_type: public rtp_paramlist_param_type
		{
		public:
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;
		};

		static rtp_param_type* params();
		static rtp_named_param_list params(ScenarioType scen);

		static sat_scenario* create_instance(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen) const = 0;
		virtual scenario_data_t get_scenario_data() const;
		virtual bool is_complete(state_t const& st);

	protected:
		// TODO: dimensionality
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_ang_vel;
	};
/*
	template < WorldDimensionality dim >
	class angular_full_stop: public sat_scenario< dim >
	{
	public:
		using sat_scenario< dim >::state_t;

		enum Params {
			NumParams,
		};

		static rtp_named_param_list params();

		angular_full_stop(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen);
	};

	template < WorldDimensionality dim >
	class linear_full_stop: public sat_scenario< dim >
	{
	public:
		using sat_scenario< dim >::state_t;

		enum Params {
			NumParams,
		};

		static rtp_named_param_list params();

		linear_full_stop(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen);
	};
*/
}


#endif

