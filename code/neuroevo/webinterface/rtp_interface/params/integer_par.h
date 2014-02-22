// integer_par.h

#ifndef __NE_RTP_INTEGER_PARAM_H
#define __NE_RTP_INTEGER_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"

#include <Wt/WLineEdit>
#include <Wt/WIntValidator>
#include <Wt/WLocale>


class rtp_integer_param_type: public rtp_param_type
{
public:
	rtp_integer_param_type(int default_val = 0, int min_val = std::numeric_limits< int >::min(), int max_val = std::numeric_limits< int >::max()):
		m_default(default_val),
		m_min(min_val),
		m_max(max_val)
	{}

public:
	virtual boost::any default_value() const
	{
		return m_default;
	}

	virtual i_param_widget* create_widget() const
	{
		rtp_param_widget< Wt::WLineEdit >* edit = new rtp_param_widget< Wt::WLineEdit >(this);
		edit->setValidator(new Wt::WIntValidator(m_min, m_max));
		edit->setText(Wt::WString("{1}").arg(m_default));
		return edit;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		Wt::WLineEdit const* edit = (Wt::WLineEdit const*)w->get_wt_widget();
		return rtp_param(Wt::WLocale::currentLocale().toInt(edit->text()));
	}

private:
	int m_default;
	int m_min;
	int m_max;
};


#endif


