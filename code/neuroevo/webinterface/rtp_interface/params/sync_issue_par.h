// sync_issue_par.h

#ifndef __NE_SYNC_ISSUE_PARAM_H
#define __NE_SYNC_ISSUE_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"
#include "integer_par.h"
#include "nestedparam_par.h"
#include "paramlist_par.h"

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/WVBoxLayout>


class i_param_widget;

class sel_param_type: public rtp_param_type
{
public:
	virtual ~sel_param_type()
	{
		//__debugbreak();
	}

	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* w = new rtp_param_widget< Wt::WComboBox >(this);
		w->addItem("Nothing");
		w->addItem("Something");
		return w;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		bool something = box->currentIndex() == 1;
		return rtp_param(something);
	}
};

class sync_issue_widget: public Wt::WContainerWidget
{
public:
	sync_issue_widget();
};

class sync_issue_param_type_2: public rtp_param_type//rtp_nestedparam_param_type
{
public:
	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		return rtp_param();
	}

	virtual rtp_named_param provide_selection_param() const
	{
		return rtp_named_param(new sel_param_type(), "Sync Issue 2");
	}

	//virtual rtp_param_type* provide_nested_param(rtp_param_manager* mgr) const;

private:
};


typedef sync_issue_param_type_2 sync_issue_param_type;


#endif


