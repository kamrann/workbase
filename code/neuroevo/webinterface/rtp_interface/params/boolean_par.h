// boolean_par.h

#ifndef __NE_RTP_BOOLEAN_PARAM_H
#define __NE_RTP_BOOLEAN_PARAM_H

#include "../rtp_param.h"


class i_param_widget;

class rtp_boolean_param_type: public rtp_param_type
{
public:
	rtp_boolean_param_type(
		bool default_val = false):
		m_default(default_val)
	{}

	virtual ~rtp_boolean_param_type();

public:
	virtual boost::any default_value() const
	{
		return m_default;
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;

private:
	bool m_default;
};


#endif


