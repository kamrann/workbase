// nestedparam_par.h

#ifndef __NE_RTP_NESTED_PARAM_PARAM_H
#define __NE_RTP_NESTED_PARAM_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"


class rtp_nestedparam_param_type: public rtp_param_type
{
public:
	rtp_nestedparam_param_type(rtp_named_param const& sel, rtp_param_type* nested): m_selection_param(sel), m_nested_param(nested)
	{}

public:
	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget() const
	{
		rtp_nested_param_widget* w = new rtp_nested_param_widget(this);
		w->set_selection_widget(m_selection_param.type->create_widget());
		w->set_nested_widget(m_nested_param->create_widget());
		return w;
	}

	virtual rtp_param get_widget_param(i_param_widget* w) const
	{
		rtp_param_list_widget* list_w = (rtp_param_list_widget*)w;
		rtp_param_list p;
		for(size_t i = 0; i < list_w->num_children(); ++i)
		{
			i_param_widget* c = list_w->get_child(i);
			p.push_back(c->get_param());
		}
		return rtp_param(p);
	}

private:
	rtp_named_param m_selection_param;
	rtp_param_type* m_nested_param;
};


#endif


