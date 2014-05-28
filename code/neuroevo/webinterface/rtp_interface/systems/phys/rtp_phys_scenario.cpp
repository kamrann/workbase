// rtp_phys_scenario.cpp

#include "rtp_phys_scenario.h"
#include "scenarios/ground_based.h"
#include "scenarios/space_based.h"
#include "../../params/realnum_par.h"

#include <Wt/WComboBox>

#include <boost/math/constants/constants.hpp>


namespace rtp_phys {

	const std::string phys_scenario_base::Names[] = {
		"Ground Based",
		"Space Based",
	};

	phys_scenario::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	
	size_t phys_scenario::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return 0;
	}

	rtp_named_param phys_scenario::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		// TODO:
		return rtp_named_param();
	}

	rtp_param_type* phys_scenario::params()
	{
		rtp_named_param_list p;
		p.push_back(rtp_named_param(new phys_scenario::enum_param_type(), "Scenario"));
		p.push_back(rtp_named_param(
			new rtp_realnum_param_type(
				10.0,
				0.0,
				1000.0
			),
			"Duration"));
		return new rtp_staticparamlist_param_type(p);
	}
/*
	rtp_named_param_list phys_scenario::params(Type scen)
	{
		switch(scen)
		{
			case TestScenario:
			return test_scenario::params();
		
			default:
			assert(false);
			return rtp_named_param_list();
		}
	}
*/
	phys_scenario* phys_scenario::create_instance(rtp_param param)
	{
		auto param_list = boost::any_cast<rtp_param_list>(param);

		Type scenario_type = boost::any_cast<Type>(param_list[0]);
		phys_scenario* scenario = nullptr;
		switch(scenario_type)
		{
			case GroundBased:
			scenario = new ground_based_scenario(rtp_param());
			break;
	
			case SpaceBased:
			scenario = new space_based_scenario(rtp_param());
			break;

			default:
			assert(false);
		}

		scenario->m_duration = boost::any_cast<double>(param_list[1]);
		
		return scenario;
	}

	phys_scenario::scenario_data_t phys_scenario::get_scenario_data() const
	{
		return scenario_data_t();
	}

	bool phys_scenario::is_complete(state_t const& st)
	{
		return st.time >= m_duration;
	}
}

