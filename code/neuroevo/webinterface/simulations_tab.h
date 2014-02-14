// simulations_tab.h

#ifndef __SIMULATIONS_TAB_H
#define __SIMULATIONS_TAB_H

#include <Wt/WContainerWidget>
#include <Wt/Dbo/ptr>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


using namespace Wt;

class Wt::WLineEdit;
class Wt::WPushButton;
class Wt::WTextArea;
class Wt::WTableView;

class WebInterfaceApplication;

class evo_period;


class SimulationsTab: public WContainerWidget
{
private:
	struct sim_params
	{
		size_t pop_size;
		size_t num_epochs;
	};

public:
	SimulationsTab(WContainerWidget* parent = nullptr);

private:
	WLineEdit* pop_size_edit;
	WLineEdit* num_epochs_edit;
	WPushButton* run_sim_btn;
	WTextArea* txt_output;
	WTableView* observations_table;

	boost::shared_ptr< boost::thread > sim_thread;

private:
	void evo_started_cb(Wt::Dbo::ptr< evo_period > ep);
	void generation_cb(WStandardItemModel* obs_model, std::string txt);
	void completion_cb(std::string txt);
	void run_simulation_threadmain(WebInterfaceApplication* app, sim_params params);

public:
	void on_run_simulation();
};


#endif

