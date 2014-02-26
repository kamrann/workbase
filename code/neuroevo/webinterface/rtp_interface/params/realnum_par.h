// realnum_par.h

#ifndef __NE_RTP_REALNUM_PARAM_H
#define __NE_RTP_REALNUM_PARAM_H

#include "../rtp_param.h"


class i_param_widget;

class rtp_realnum_param_type: public rtp_param_type
{
public:
	rtp_realnum_param_type(double default_val = 0.0, double min_val = -std::numeric_limits< double >::max(), double max_val = std::numeric_limits< double >::max()):
		m_default(default_val),
		m_min(min_val),
		m_max(max_val),
		m_use_spin(true),
		m_spin_step(1.0)
	{}

public:
	inline void use_spin(bool use, double step = 1.0)
	{
		m_use_spin = use;
		if(use)
		{
			m_spin_step = step;
		}
	}

	virtual boost::any default_value() const
	{
		return m_default;
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;

private:
	double m_default;
	double m_min;
	double m_max;
	bool m_use_spin;
	double m_spin_step;
};


#endif


