// system_test_tab.cpp

#include "system_test_tab.h"

#include "rtp_interface/systems/rtp_system.h"

#include "generic_system_coordinator.h"

#include "wt_param_widgets/param_accessor.h"

#include <yaml-cpp/yaml.h>

#include <Wt/WHBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WPanel>
#include <Wt/WGroupBox>
#include <Wt/WMessageBox>

#include <boost/random/mersenne_twister.hpp>

#include <filesystem>
#include <regex>


namespace fs = std::tr2::sys;

std::vector< std::string > find_presets(std::string filter_str)
{
	std::vector< std::string > presets;

	filter_str = std::string(R"(.*\.)") + filter_str;
	std::regex rgx(filter_str);

	auto path = fs::current_path< fs::path >();
	for(auto it = fs::directory_iterator(path); it != fs::directory_iterator(); ++it)
	{
		auto const& file = it->path();

		if(!fs::is_directory(file) && std::regex_search(file.filename(), rgx))
		{
			presets.emplace_back(file.filename());
		}
	}

	return presets;
}

SystemTestTab::SystemTestTab(WContainerWidget* parent): WContainerWidget(parent)
{
	system_widget = nullptr;

	vlayout = new WVBoxLayout();
	setLayout(vlayout);

#ifdef _DEBUG
	auto dump_btn = new Wt::WPushButton("Dump Params");
	vlayout->addWidget(dump_btn, 0, Wt::AlignLeft);
	dump_btn->clicked().connect(std::bind([=]
	{
		std::cout << "Param dump:" << std::endl << system_params_tree->get_yaml_param(false) << std::endl;
	}));
#endif

	auto spm = std::make_shared< prm::schema::schema_provider_map >();
	prm::qualified_path abs_path;
	auto root = rtp::i_system::update_schema_providor(spm, abs_path, false);
	(*spm)[prm::qualified_path()] = spm->at(root);
	system_params_tree = prm::param_tree::create(
		[spm](prm::qualified_path const& path, prm::param_accessor acc)
	{
		return spm->at(path.unindexed())(acc);
	});

//	WPanel* sys_params_panel = new WPanel;
//	sys_params_panel->setTitle("System parameters");
//	sys_params_panel->setCentralWidget(system_params_tree);
//	vlayout->addWidget(sys_params_panel, 0, Wt::AlignLeft);

	vlayout->addWidget(system_params_tree, 0, Wt::AlignLeft);

	Wt::WPushButton* btn = new Wt::WPushButton("Start");
	vlayout->addWidget(btn, 0, Wt::AlignLeft);

	btn->clicked().connect(this, &SystemTestTab::on_system_changed);

	hlayout = new WHBoxLayout();
	vlayout->addLayout(hlayout, 1);

	stop_button = new WPushButton("Stop");
	stop_button->setHidden(true);
	vlayout->addWidget(stop_button, 0, Wt::AlignLeft);

	stop_button->clicked().connect(this, &SystemTestTab::on_stop);

	restart_button = new WPushButton("Restart");
	restart_button->setHidden(true);
	vlayout->addWidget(restart_button, 0, Wt::AlignLeft);

	restart_button->clicked().connect(this, &SystemTestTab::on_restart);
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
	auto params = system_params_tree->get_param_accessor();
	try
	{
		auto sys_factory = rtp::i_system::generate_system_factory(params);
		auto agents_data = rtp::i_system::generate_agents_data(params, false);

		coordinator = std::make_unique< generic_sys_coordinator >(
			std::move(sys_factory),
			std::move(agents_data));
		std::pair< WWidget*, WWidget* > sys_widgets = coordinator->initialize();
		set_system_widget(sys_widgets.first);
		set_history_widget(sys_widgets.second);
		
		coordinator->restart();

		stop_button->setHidden(false);
		restart_button->setHidden(true);
	}
	catch(prm::required_unspecified e)
	{
		std::stringstream msg;
		msg << "Unspecified paramater: " << e.what();
		Wt::WMessageBox::show("Error", msg.str(), Wt::Ok);
	}
}

void SystemTestTab::on_stop()
{
	coordinator->cancel();

	stop_button->setHidden(true);
	restart_button->setHidden(false);
}

void SystemTestTab::on_restart()
{
	coordinator->restart();

	restart_button->setHidden(true);
	stop_button->setHidden(false);
}

