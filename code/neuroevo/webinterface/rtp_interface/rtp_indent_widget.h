// rtp_indent_widget.h

#ifndef __NE_RTP_INDENT_WIDGET_H
#define __NE_RTP_INDENT_WIDGET_H

#include "rtp_param_widget.h"

#include <Wt/WContainerWidget>


class rtp_indent_widget: //public rtp_param_widget< Wt::WContainerWidget >
	public Wt::WContainerWidget
{
public:
	typedef Wt::WContainerWidget base_t;

public:
	rtp_indent_widget(){}//(rtp_paramlist_param_type const* _type);
//	virtual ~rtp_param_list_widget();

public:
	void set_child(i_param_widget* c)
	{
		this->setPadding(20, Wt::Left);

		addWidget(c->get_wt_widget());

		child = c;
	}

private:
	i_param_widget* child;
};



#endif


