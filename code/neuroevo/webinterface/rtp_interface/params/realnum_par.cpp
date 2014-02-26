// realnum_par.cpp

#include "realnum_par.h"
#include "../rtp_param_widget.h"

//#include <Wt/WLineEdit>
//#include <Wt/WDoubleValidator>
//#include <Wt/WLocale>
#include <Wt/WDoubleSpinBox>


i_param_widget* rtp_realnum_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WDoubleSpinBox >* edit = new rtp_param_widget< Wt::WDoubleSpinBox >(this);
	edit->setStyleClass("wb_numerical_input");
	edit->setSingleStep(m_spin_step);
	//edit->setValidator(new Wt::WDoubleValidator(m_min, m_max));
	edit->setRange(m_min, m_max);
	//edit->setText(Wt::WString("{1}").arg(m_default));
	edit->setValue(m_default);
	return edit;
}

rtp_param rtp_realnum_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WDoubleSpinBox const* edit = (Wt::WDoubleSpinBox const*)w->get_wt_widget();
	return rtp_param(edit->value());
		//Wt::WLocale::currentLocale().toDouble(edit->text()));
}


