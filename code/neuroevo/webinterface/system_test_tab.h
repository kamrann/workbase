// system_test_tab.h

#ifndef __SYSTEM_TEST_TAB_H
#define __SYSTEM_TEST_TAB_H

#include "system_coordinator.h"

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WVBoxLayout>


using namespace Wt;


class SystemTestTab: public WContainerWidget
{
public:
	SystemTestTab(WContainerWidget* parent = nullptr);

public:
	void set_system_widget(WWidget* w);

private:
	void on_system_changed();
	void on_restart();

public:	// temp public
	system_coordinator* coordinator;

private:
	WVBoxLayout* vlayout;
	Wt::WComboBox* system_box;
	WWidget* system_widget;
};


#endif

