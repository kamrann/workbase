// fixed_or_random_par.h

#ifndef __NE_RTP_FIXED_OR_RANDOM_PARAM_H
#define __NE_RTP_FIXED_OR_RANDOM_PARAM_H

#include "../rtp_param.h"
#include "util/fixed_or_random.h"


class i_param_widget;

class rtp_fixed_or_random_param_type: public rtp_param_type
{
public:
	rtp_fixed_or_random_param_type(
		double default_val = 0.0,
		double default_min = 0.0,
		double default_max = 1.0,
		double min_val = -std::numeric_limits< double >::max(),
		double max_val = std::numeric_limits< double >::max()
		):
		m_default(default_val),
		m_default_min(default_min),
		m_default_max(default_max),
		m_min(min_val),
		m_max(max_val)
	{}

public:
	virtual boost::any default_value() const
	{
		return m_default;
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;

private:
	double m_default;
	double m_default_min;
	double m_default_max;
	double m_min;
	double m_max;
};


#endif


