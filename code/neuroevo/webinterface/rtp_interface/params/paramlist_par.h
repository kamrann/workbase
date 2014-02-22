// param_list_par.h

#ifndef __NE_RTP_PARAM_LIST_PARAM_H
#define __NE_RTP_PARAM_LIST_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"


class rtp_paramlist_param_type: public rtp_param_type
{
public:
	rtp_paramlist_param_type(rtp_named_param_list const& params): m_params(params)
	{}

public:
	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget() const
	{
		rtp_param_list_widget* w = new rtp_param_list_widget(this);
		for(rtp_named_param const& p : m_params)
		{
			w->add_child(p.name, p.type->create_widget());
		}
		return w;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		rtp_param_list_widget const* list_w = (rtp_param_list_widget const*)w;
		rtp_param_list p;
		for(size_t i = 0; i < list_w->num_children(); ++i)
		{
			i_param_widget const* c = list_w->get_child(i);
			p.push_back(c->get_param());
		}
		return rtp_param(p);
	}

private:
	rtp_named_param_list m_params;
};


#endif


