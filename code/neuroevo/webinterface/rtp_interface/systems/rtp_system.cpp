// rtp_system.cpp

#include "rtp_system.h"
#include "../rtp_param_widget.h"
#include "../params/paramlist_par.h"
#include "../params/dimensionality_par.h"

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

i_param_widget* system_type_param_type::create_widget() const
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


boost::any system_param_type::default_value() const
{
	return boost::any();
}

i_param_widget* system_param_type::create_widget() const
{
	rtp_nested_param_widget* cont = new rtp_nested_param_widget(this);
	system_type_param_type* stpt = new system_type_param_type;
	cont->set_selection_widget(stpt->create_widget());

	Wt::WComboBox* box = (Wt::WComboBox*)cont->get_selection_widget()->get_wt_widget();
	box->changed().connect(std::bind([=]()
	{
		Wt::WAbstractItemModel* model = box->model();
		int idx = box->currentIndex();
		SystemType system = boost::any_cast< SystemType >(model->data(idx, 0, Wt::UserRole));
		rtp_named_param_list sub_params = i_system::params(system);
		rtp_paramlist_param_type* paramlist_prm = new rtp_paramlist_param_type(sub_params);
		cont->set_nested_widget(paramlist_prm->create_widget());
	}));
	box->changed().emit();

	return cont;
}

rtp_param system_param_type::get_widget_param(i_param_widget const* w) const
{
	rtp_nested_param_widget const* nested_w = (rtp_nested_param_widget const*)w;
	return std::make_pair(
		nested_w->get_selection_widget()->get_param(),
		nested_w->get_nested_widget()->get_param()
		);
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

rtp_named_param_list i_system::params(SystemType sys)
{
	switch(sys)
	{
		//	case NoughtsAndCrosses:
		//		return rtp_system_traits< NoughtsAndCrosses >::params();

	case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::params();

	default:
		return rtp_named_param_list();
	}
}

i_system* i_system::create_instance(rtp_param param)
{
	auto param_pr = boost::any_cast<std::pair< rtp_param, rtp_param >>(param);
	SystemType type = boost::any_cast<SystemType>(param_pr.first);
	switch(type)
	{
	case NoughtsAndCrosses:
		return nullptr;
	case ShipAndThrusters2D:
		return new rtp_sat::sat_system< WorldDimensionality::dim2D >(param_pr.second);
			//sat_system_base::create_instance(param_pr.second);
	default:
		return nullptr;
	}
}

