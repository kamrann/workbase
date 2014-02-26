// system_test_tab.cpp

#include "system_test_tab.h"

#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/rtp_param_widget.h"

#include "systems/noughts_and_crosses/scenarios/play_to_completion.h"
#include "nac_system_coordinator.h"

#include "systems/ship_and_thrusters/scenarios/full_stop.h"
#include "ship_system_coordinator.h"

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

	system_param_type* spt = new system_param_type();
	system_params_widget = spt->create_widget(&param_mgr);
	sys_params_panel->setCentralWidget(system_params_widget->get_wt_widget());

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
	rtp_param sys_param = system_params_widget->get_param();
	i_system* sys = std::get< 0 >(i_system::create_instance(sys_param));

	// TEMP
	if(boost::any_cast<SystemType>(boost::any_cast< std::pair< boost::any, boost::any > >(sys_param).first) != ShipAndThrusters2D)
	{
		return;
	}
	coordinator = new ship_coordinator< WorldDimensionality::dim2D >(sys);
	//

	std::pair< WWidget*, WWidget* > w = coordinator->initialize();
	set_system_widget(w.first);
	set_history_widget(w.second);
	coordinator->restart();
}

void SystemTestTab::on_restart()
{
	coordinator->restart();
}

