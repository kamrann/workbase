// rtp_system.cpp

#include "rtp_system.h"
#include "../rtp_param_widget.h"
#include "../params/paramlist_par.h"
#include "../params/dimensionality_par.h"
#include "../rtp_param_manager.h"

//#include "nac/rtp_nac_system.h"
#include "sat/rtp_sat_system.h"

#include <Wt/WComboBox>
#include <Wt/WContainerWidget>


const std::array< std::string, NumSystems > SystemNames = {
	"Noughts & Crosses",
	"Ship & Thrusters 2D",
};


boost::any system_type_param_type::default_value() const
{
	return NoughtsAndCrosses;
}

i_param_widget* system_type_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);

	for(size_t i = 0; i < NumSystems; ++i)
	{
		box->addItem(SystemNames[i]);
		box->model()->setData(i, 0, (SystemType)i, Wt::UserRole);
	}

	return box;
}

rtp_param system_type_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
	Wt::WAbstractItemModel const* model = box->model();
	SystemType system = boost::any_cast< SystemType >(model->data(box->currentIndex(), 0, Wt::UserRole));
	return rtp_param(system);
}


system_param_type::system_param_type(bool evolvable)
{
	m_evolvable = evolvable;
}

rtp_named_param system_param_type::provide_selection_param() const
{
	return rtp_named_param(new system_type_param_type(), "System Type");
}

rtp_param_type* system_param_type::provide_nested_param(rtp_param_manager* mgr) const
{
	SystemType system = boost::any_cast<SystemType>(mgr->retrieve_param("System Type"));
	rtp_named_param_list sub_params = i_system::params(system, m_evolvable);
	return new rtp_staticparamlist_param_type(sub_params);
}


/*
//template <>
rtp_named_param_list rtp_system_traits< NoughtsAndCrosses >::params()
{
	rtp_named_param_list p;
	p.push_back(rtp_named_param(new rtp_nac::nac_scenario::param_type(), "Scenario"));
	return p;
}

//template <>
rtp_named_param_list rtp_system_traits< ShipAndThrusters2D >::params()
{
	rtp_named_param_list p;
//	p.push_back(rtp_named_param(new rtp_dimensionality_param_type(), "Dimensions"));
	p.push_back(rtp_named_param(new rtp_sat::sat_scenario< WorldDimensionality::dim2D >::param_type(), "Scenario"));
	return p;
}
*/

rtp_named_param_list i_system::params(SystemType sys, bool evolvable)
{
	switch(sys)
	{
		//	case NoughtsAndCrosses:
		//		return rtp_system_traits< NoughtsAndCrosses >::params();

	case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::params(evolvable);

	default:
		return rtp_named_param_list();
	}
}

std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > i_system::create_instance(rtp_param param, bool evolvable)
{
	auto param_pr = boost::any_cast<std::pair< rtp_param, rtp_param >>(param);
	SystemType type = boost::any_cast<SystemType>(param_pr.first);
	switch(type)
	{
	case NoughtsAndCrosses:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);
	case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_pr.second, evolvable);
	default:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);
	}
}

