// system_test_tab.cpp

#include "system_test_tab.h"

#include <Wt/WHBoxLayout>
#include <Wt/WPushButton>

#include "systems/noughts_and_crosses/scenarios/play_to_completion.h"
#include "nac_system_coordinator.h"

#include "systems/ship_and_thrusters/scenarios/full_stop.h"
#include "ship_system_coordinator.h"

#include <boost/random/mersenne_twister.hpp>


SystemTestTab::SystemTestTab(WContainerWidget* parent): WContainerWidget(parent)
{
	system_widget = nullptr;

	vlayout = new WVBoxLayout();
	setLayout(vlayout);

	system_box = new WComboBox();
	system_box->addItem("Noughts & Crosses");
	system_box->addItem("Ship & Thrusters");
	vlayout->addWidget(system_box);

	system_box->changed().connect(this, &SystemTestTab::on_system_changed);

	WPushButton* restart_btn = new WPushButton("Restart");
	vlayout->addWidget(restart_btn);

	restart_btn->clicked().connect(this, &SystemTestTab::on_restart);

	on_system_changed();
}

void SystemTestTab::set_system_widget(WWidget* w)
{
	int index = vlayout->count() - 1;
	vlayout->insertWidget(index, w, 1);

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

void SystemTestTab::on_system_changed()
{
	switch(system_box->currentIndex())
	{
	case 0:
		{
			// NAC
			typedef play_to_completion< 2, 3, false, boost::random::mt19937 > scenario_t;
			coordinator = new nac_coordinator< scenario_t >;
		}
		break;

	case 1:
		{
			// Ship
			typedef full_stop< WorldDimensionality::dim2D > scenario_t;
			coordinator = new ship_coordinator< scenario_t >;
		}
		break;
	}

	WWidget* w = coordinator->initialize();
	set_system_widget(w);
	coordinator->restart();
}

void SystemTestTab::on_restart()
{
	coordinator->restart();
}

