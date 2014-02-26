// nestedparam_par.h

#ifndef __NE_RTP_NESTED_PARAM_PARAM_H
#define __NE_RTP_NESTED_PARAM_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"
#include "../rtp_param_manager.h"


class rtp_nestedparam_param_type: public rtp_param_type
{
public:
	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const
	{
		rtp_nested_param_widget* w = new rtp_nested_param_widget(this);
		w->set_selection_widget(provide_selection_param().type->create_widget(mgr));
		w->set_nested_widget(provide_nested_param(mgr)->create_widget(mgr));
		return w;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		rtp_nested_param_widget const* nested_w = (rtp_nested_param_widget const*)w;
		return std::make_pair(
			nested_w->get_selection_widget()->get_param(),
			nested_w->get_nested_widget()->get_param()
			);
	}

	virtual rtp_named_param provide_selection_param() const = 0;
	virtual rtp_param_type* provide_nested_param(rtp_param_manager* mgr) const = 0;
};


class rtp_staticnestedparam_param_type: public rtp_nestedparam_param_type
{
public:
	rtp_staticnestedparam_param_type(rtp_named_param const& sel, rtp_param_type* nested): m_selection_param(sel), m_nested_param(nested)
	{}

	virtual rtp_named_param provide_selection_param() const
	{
		return m_selection_param;
	}

	virtual rtp_param_type* provide_nested_param(rtp_param_manager* mgr) const
	{
		return m_nested_param;
	}

private:
	rtp_named_param m_selection_param;
	rtp_param_type* m_nested_param;
};

class rtp_autonestedparam_param_type: public rtp_nestedparam_param_type
{
public:
	typedef rtp_nestedparam_param_type base_t;

public:
	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const
	{
		rtp_nested_param_widget* w = new rtp_nested_param_widget(this);
		rtp_named_param sel_prm = provide_selection_param();
		i_param_widget* sel_w = sel_prm.type->create_widget(mgr);
		mgr->register_widget(sel_prm.name, sel_w);
		w->set_selection_widget(sel_w);
		//w->set_nested_widget(provide_nested_param(mgr)->create_widget(mgr));

		boost::function< void() > handler = [=]()
		{
			w->set_nested_widget(provide_nested_param(mgr)->create_widget(mgr));
		};
		mgr->register_change_handler(sel_prm.name, handler);
		
		handler();
		return w;
	}
};


#endif


