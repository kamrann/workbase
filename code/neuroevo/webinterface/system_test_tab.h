// system_test_tab.h

#ifndef __SYSTEM_TEST_TAB_H
#define __SYSTEM_TEST_TAB_H

#include "system_coordinator.h"

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>

#include <memory>


using namespace Wt;

namespace prm {
	class param_tree;
}

class SystemTestTab: public WContainerWidget
{
public:
	SystemTestTab(WContainerWidget* parent = nullptr);

public:
	void set_system_widget(WWidget* w);
	void set_history_widget(WWidget* w);

private:
	void on_system_changed();
	void on_stop();
	void on_restart();

public:	// temp public
	std::unique_ptr< system_coordinator > coordinator;

private:
	WVBoxLayout* vlayout;
	WHBoxLayout* hlayout;
	prm::param_tree* system_params_tree;

	WWidget* system_widget;
	WWidget* history_widget;

	WPushButton* stop_button;
	WPushButton* restart_button;
};


#endif

