// system_test_tab.h

#ifndef __SYSTEM_TEST_TAB_H
#define __SYSTEM_TEST_TAB_H

#include "system_coordinator.h"

#include "rtp_interface/rtp_param_manager.h"

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>


using namespace Wt;

class i_param_widget;


class SystemTestTab: public WContainerWidget
{
public:
	SystemTestTab(WContainerWidget* parent = nullptr);

public:
	void set_system_widget(WWidget* w);
	void set_history_widget(WWidget* w);

private:
	void on_system_changed();
	void on_restart();

public:	// temp public
	system_coordinator* coordinator;

private:
	WVBoxLayout* vlayout;
	WHBoxLayout* hlayout;
	i_param_widget* system_params_widget;
	rtp_param_manager param_mgr;
	WWidget* system_widget;
	WWidget* history_widget;
};


#endif

