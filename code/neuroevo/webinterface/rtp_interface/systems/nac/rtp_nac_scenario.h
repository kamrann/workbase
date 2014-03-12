// rtp_nac_scenario.h

#ifndef __NE_RTP_NAC_SCENARIO_H
#define __NE_RTP_NAC_SCENARIO_H

#include "../rtp_scenario.h"
#include "../../rtp_param.h"
#include "../../params/nestedparam_par.h"
#include "../../params/paramlist_par.h"

#include <array>


namespace Wt {
	class WWidget;
}

namespace rtp_nac
{
	class nac_scenario: public i_scenario
	{
	public:
		enum ScenarioType {
			PlayToCompletion,

			NumScenarios,
		};

		static const std::array< std::string, NumScenarios > ScenarioNames;

		class enum_param_type: public rtp_param_type
		{
		public:
			virtual boost::any default_value() const;
			virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
			virtual rtp_param get_widget_param(i_param_widget const* w) const;
		};

		class param_type: public rtp_paramlist_param_type//rtp_nestedparam_param_type
		{
		public:
//			virtual rtp_named_param provide_selection_param() const;
//			virtual rtp_param_type* provide_nested_param(rtp_param_manager* mgr) const;
			virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
			virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;
		};

	public:
		static rtp_named_param_list params(ScenarioType scen);
	};

	class play_to_completion: public nac_scenario
	{
	public:
		enum Params {
			NumParams,
		};

		static rtp_named_param_list params();
	};
}


#endif

