// rtp_nac_scenario.cpp

#include "rtp_nac_scenario.h"
#include "../../rtp_param.h"
#include "../../params/paramlist_par.h"

#include <Wt/WComboBox>


namespace rtp_nac {

	const std::array< std::string, nac_scenario::NumScenarios > nac_scenario::ScenarioNames = {
		"Play To Completion",
	};

	boost::any nac_scenario::enum_param_type::default_value() const
	{
		return PlayToCompletion;
	}

	i_param_widget* nac_scenario::enum_param_type::create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

		for(size_t i = 0; i < NumScenarios; ++i)
		{
			box->addItem(ScenarioNames[i]);
			box->model()->setData(i, 0, (ScenarioType)i, Wt::UserRole);
		}

		return box;
	}

	rtp_param nac_scenario::enum_param_type::get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel const* model = box->model();
		ScenarioType system = boost::any_cast< ScenarioType >(model->data(box->currentIndex(), 0, Wt::UserRole));
		return rtp_param(system);
	}

	rtp_named_param nac_scenario::param_type::provide_selection_param() const
	{
		return rtp_named_param(new nac_scenario::enum_param_type(), "Scenario Type");
	}

	rtp_param_type* nac_scenario::param_type::provide_nested_param(rtp_param_manager* mgr) const
	{
		ScenarioType scenario = boost::any_cast< ScenarioType >(mgr->retrieve_param("Scenario Type"));
		rtp_named_param_list sub_params = nac_scenario::params(scenario);
		return new rtp_staticparamlist_param_type(sub_params);
	}
/*
	boost::any nac_scenario::param_type::default_value() const
	{
		return boost::any();
	}

	i_param_widget* nac_scenario::param_type::create_widget() const
	{
		rtp_nested_param_widget* cont = new rtp_nested_param_widget(this);
		nac_scenario::enum_param_type* stpt = new nac_scenario::enum_param_type;
		cont->set_selection_widget(stpt->create_widget());

		Wt::WComboBox* box = (Wt::WComboBox*)cont->get_selection_widget()->get_wt_widget();
		box->changed().connect(std::bind([=]()
		{
			Wt::WAbstractItemModel* model = box->model();
			int idx = box->currentIndex();
			ScenarioType scenario = boost::any_cast< ScenarioType >(model->data(idx, 0, Wt::UserRole));
			rtp_named_param_list sub_params = nac_scenario::params(scenario);
			rtp_paramlist_param_type* paramlist_prm = new rtp_paramlist_param_type(sub_params);
			cont->set_nested_widget(paramlist_prm->create_widget());
		}));
		box->changed().emit();

		return cont;
	}

	rtp_param nac_scenario::param_type::get_widget_param(i_param_widget const* w) const
	{
		rtp_nested_param_widget const* nested_w = (rtp_nested_param_widget const*)w;
		return std::make_pair(
			nested_w->get_selection_widget()->get_param(),
			nested_w->get_nested_widget()->get_param()
			);
	}
*/

	rtp_named_param_list nac_scenario::params(ScenarioType scen)
	{
		switch(scen)
		{
		case PlayToCompletion:
			return play_to_completion::params();
		default:
			assert(false);
			return rtp_named_param_list();
		}
	}


	rtp_named_param_list play_to_completion::params()
	{
		rtp_named_param_list p;
		return p;
	}

}


