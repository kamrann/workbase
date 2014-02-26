// integer_par.cpp

#include "integer_par.h"
#include "../rtp_param_widget.h"

#include <Wt/WSpinBox>
//#include <Wt/WIntValidator>
//#include <Wt/WLocale>


i_param_widget* rtp_integer_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WSpinBox >* edit = new rtp_param_widget< Wt::WSpinBox >(this);
	edit->setStyleClass("wb_numerical_input");
//	edit->resize(50, Wt::WLength::Auto);
	edit->setSingleStep(m_spin_step);
//	edit->setValidator(new Wt::WIntValidator(m_min, m_max));
	edit->setRange(m_min, m_max);
//	edit->setText(Wt::WString("{1}").arg(m_default));
	edit->setValue(m_default);
	edit->changed().connect(std::bind([edit](){
		// Do nothing
		std::string txt = edit->text().toUTF8();
		int y, x = 0;
		y = x;
	}));
	return edit;
}

rtp_param rtp_integer_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WSpinBox const* edit = (Wt::WSpinBox const*)w->get_wt_widget();
	return rtp_param(edit->value());
		//Wt::WLocale::currentLocale().toInt(edit->text()));
}


