// integer_par.h

#ifndef __NE_RTP_INTEGER_PARAM_H
#define __NE_RTP_INTEGER_PARAM_H

#include "../rtp_param.h"


class i_param_widget;

class rtp_integer_param_type: public rtp_param_type
{
public:
	rtp_integer_param_type(
		int default_val = 0,
		int min_val = std::numeric_limits< int >::min(),
		int max_val = std::numeric_limits< int >::max()):
		m_default(default_val),
		m_min(min_val),
		m_max(max_val),
		m_use_spin(true),
		m_spin_step(1)
	{}

public:
	inline void use_spin(bool use, int step = 1)
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
	int m_default;
	int m_min;
	int m_max;
	bool m_use_spin;
	int m_spin_step;
};


#endif


