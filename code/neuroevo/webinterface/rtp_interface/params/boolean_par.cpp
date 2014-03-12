// boolean_par.cpp

#include "boolean_par.h"
#include "../rtp_param_widget.h"

#include <Wt/WCheckBox>


rtp_boolean_param_type::~rtp_boolean_param_type()
{

}

i_param_widget* rtp_boolean_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WCheckBox >* box = new rtp_param_widget< Wt::WCheckBox >(this);
	box->setCheckState(m_default ? Wt::CheckState::Checked : Wt::CheckState::Unchecked);
	return box;
}

rtp_param rtp_boolean_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WCheckBox const* box = (Wt::WCheckBox const*)w->get_wt_widget();
	return rtp_param(box->isChecked());
}


