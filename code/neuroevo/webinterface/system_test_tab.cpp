// system_test_tab.cpp

#include "system_test_tab.h"

#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/rtp_param_widget.h"

#include "generic_system_coordinator.h"

#include <yaml-cpp/yaml.h>

#include <Wt/WHBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WPanel>

#include <boost/random/mersenne_twister.hpp>


SystemTestTab::SystemTestTab(WContainerWidget* parent): WContainerWidget(parent)
{
	system_widget = nullptr;

	vlayout = new WVBoxLayout();
	setLayout(vlayout);

	WPanel* sys_params_panel = new WPanel;
	sys_params_panel->setTitle("System parameters");

//	rtp_param_type* spt = i_system::params(false);
//	system_params_widget = spt->create_widget(&param_mgr);

	system_params_tree = prm::param_tree::create(
		std::bind(&i_system::get_schema, std::placeholders::_1, false));

	sys_params_panel->setCentralWidget(system_params_tree);
		//system_params_widget->get_wt_widget());

	vlayout->addWidget(sys_params_panel);

//	system_box->changed().connect(this, &SystemTestTab::on_system_changed);
	Wt::WPushButton* btn = new Wt::WPushButton("Start");
	vlayout->addWidget(btn);

	btn->clicked().connect(this, &SystemTestTab::on_system_changed);

	hlayout = new WHBoxLayout();
	vlayout->addLayout(hlayout, 1);

	WPushButton* restart_btn = new WPushButton("Restart");
	vlayout->addWidget(restart_btn);

	restart_btn->clicked().connect(this, &SystemTestTab::on_restart);

	on_system_changed();
}

void SystemTestTab::set_system_widget(WWidget* w)
{
	int index = 0;
	hlayout->insertWidget(index, w, 1);

	if(system_widget != nullptr)
	{
		removeWidget(system_widget);
	}
	system_widget = w;
	//
	//((WPaintedWidget*)system_widget)->update();
	//system_widget->setWidth(400);
	system_widget->setHeight(400);
	//
}

void SystemTestTab::set_history_widget(WWidget* w)
{
	if(w != nullptr)
	{
		int index = 1;
		hlayout->insertWidget(index, w, 0);
	}

	if(history_widget != nullptr)
	{
		removeWidget(history_widget);
	}
	history_widget = w;
}

void SystemTestTab::on_system_changed()
{
/*	rtp_param sys_param = system_params_widget->get_param();
	i_system* sys = std::get< 0 >(i_system::create_instance(sys_param));
*/
	auto params = system_params_tree->get_yaml_param();
	i_system* sys = std::get< 0 >(i_system::create_instance(params));

	coordinator = new generic_sys_coordinator(sys);
	std::pair< WWidget*, WWidget* > sys_widgets = coordinator->initialize();
	set_system_widget(sys_widgets.first);
	set_history_widget(sys_widgets.second);
	coordinator->restart();
}

void SystemTestTab::on_restart()
{
	coordinator->restart();
}

