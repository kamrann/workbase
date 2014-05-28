// simulations_tab.h

#ifndef __SIMULATIONS_TAB_H
#define __SIMULATIONS_TAB_H

//#include "rtp_interface/rtp_param_manager.h"
#include "wt_param_widgets/param_manager.h"
#include "rtp_interface/rtp_properties.h"

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

class evo_run;

class i_param_widget;
class rtp_simulation;
class properties_chart_widget;


class SimulationsTab: public WContainerWidget
{
private:
	struct sim_params
	{
		size_t pop_size;
		size_t num_epochs;
		size_t trials_per_epoch;

		sim_params(): pop_size(10), num_epochs(10), trials_per_epoch(1)
		{}
	};

public:
	SimulationsTab(WContainerWidget* parent = nullptr);

private:
/*	i_param_widget* system_params_widget;
	i_param_widget* evo_params_widget;
	rtp_param_manager param_mgr;
*/
	prm::param_mgr param_mgr;
	Wt::WWidget* params_wgt;

	WPushButton* run_sim_btn;
	WTextArea* txt_output;
	WTableView* observations_table;
	properties_chart_widget* evo_chart;

	boost::shared_ptr< boost::thread > sim_thread;

private:
	void evo_started_cb(Wt::Dbo::ptr< evo_run > ep);
	void generation_cb(std::vector< boost::shared_ptr< i_property_values const > > per_gen_prop_vals, std::string txt);
	void completion_cb(std::string txt);

//	template < typename Sim >
//	void run_simulation_threadmain(WebInterfaceApplication* app, sim_params params);
	void run_simulation_threadmain(WebInterfaceApplication* app, rtp_simulation* sim);

public:
	void on_run_simulation();
};


template < typename Scenario >
struct temp_ugly_hack
{
	template < typename X >
	static void do_it(WebInterfaceApplication* app, X const& x)
	{}
};
/*
template <
	size_t Dim,
	size_t Sz
>
struct temp_ugly_hack< play_to_completion< Dim, Sz > >
{
	{
		nac::default_fixednn_solution<
			noughts_and_crosses_system< 2, 3 >,
			play_to_completion< 2, 3, false, boost::random::mt19937 >::solution_input
		> best_copy;
		best_copy.nn = best.nn;
		typedef nac_coordinator< play_to_completion< 2, 3, false, boost::random::mt19937 > > coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(
			new coordinator_t::nn_controller(best_copy),
			new coordinator_t::ui_agent_controller()
			);
	}
};
*/
/*
template <
	WorldDimensionality dim
>
struct temp_ugly_hack< full_stop< dim > >
{
	static void do_it(WebInterfaceApplication* app,
		sat::full_stop_fixednn_solution<
			ship_and_thrusters_system< dim >, typename full_stop< dim >::agent_sensor_state > const& best)
	{
		typedef ship_coordinator< full_stop< dim > > coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(
			new coordinator_t::nn_controller< sat::full_stop_fixednn_solution< ship_and_thrusters_system< dim >, typename full_stop< dim >::agent_sensor_state > >(best)
			);
	}
};

template <
	WorldDimensionality dim
>
struct temp_ugly_hack< angular_full_stop< dim > >
{
	static void do_it(WebInterfaceApplication* app,
		sat::angular_full_stop_fixednn_solution<
		ship_and_thrusters_system< dim >, typename angular_full_stop< dim >::agent_sensor_state > const& best)
	{
		typedef ship_coordinator< angular_full_stop< dim > > coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(
			new coordinator_t::nn_controller< sat::angular_full_stop_fixednn_solution< ship_and_thrusters_system< dim >, typename angular_full_stop< dim >::agent_sensor_state > >(best)
			);
	}
};

template <
	WorldDimensionality dim
>
struct temp_ugly_hack< target_orientation< dim > >
{
	static void do_it(WebInterfaceApplication* app,
		sat::target_orientation_fixednn_solution<
		ship_and_thrusters_system< dim >, typename target_orientation< dim >::agent_sensor_state > const& best)
	{
		typedef ship_coordinator< target_orientation< dim > > coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(
			new coordinator_t::nn_controller< sat::target_orientation_fixednn_solution< ship_and_thrusters_system< dim >, typename target_orientation< dim >::agent_sensor_state > >(best)
			);
	}
};


template < typename Sim >
void SimulationsTab::run_simulation_threadmain(WebInterfaceApplication* app, sim_params params)
{
	typedef Sim sim_t;

	std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
	evodb_session db_s(*app->db_cp);

	std::stringstream out;
	out.precision(4);
	std::fixed(out);

	out << ("Initialising...") << std::endl;

	// Add database entry to evo_period table
	dbo::Transaction t(db_s);
	dbo::ptr< evo_period > ep = db_s.add(new evo_period);
	ep.modify()->initial_pop_size = params.pop_size;
	t.commit();

	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::evo_started_cb, this, ep));

	sim_t sim(
		params.pop_size,
		params.trials_per_epoch
		);
	sim.init(params.num_epochs);

	sim_t::observation_data_t observations;

	out << ("Training...") << std::endl;

	size_t const GenerationBatchSize =
#ifdef _DEBUG
		5
#else
		50
#endif
		;
	std::vector< generation > gen_data(GenerationBatchSize);
	std::vector< genome > ind_data(params.pop_size * GenerationBatchSize);
	size_t gen_batch_index = 0;
	size_t ind_batch_index = 0;

	// TODO: Assuming double
	sim_t::processed_obj_val_t highest_ov;
	sim_t::agent_t best;
	for(size_t e = 0; e < params.num_epochs; ++e)
	{
		observations.clear();	// TODO:
		sim.run_epoch(observations, out);

		sim_t::agent_t best_of_generation;
		sim_t::processed_obj_val_t obj_val = sim.get_fittest(best_of_generation);
		if(e == 0 || obj_val > highest_ov)
		{
			highest_ov = obj_val;
			best = best_of_generation;
		}

		// Add database entries
		gen_data[gen_batch_index].index = sim.ga.generation;
		gen_data[gen_batch_index].genotype_diversity = sim.population_genotype_diversity();
		gen_data[gen_batch_index].period = ep;
		++gen_batch_index;

		std::sort(sim.ga.population.begin(), sim.ga.population.end());
		for(size_t i = 0; i < sim.ga.population.size(); ++i)
		{
			size_t rank = sim.ga.population.size() - i - 1;
			ind_data[ind_batch_index].gen_obj_rank = rank;
			// TODO: ind_data[ind_batch_index].encoding = sim.ga.population[rank].genome;
			++ind_batch_index;
		}

		if(gen_batch_index == GenerationBatchSize)
		{
#ifndef _DEBUG
			std::cout << "Batching new generation records..." << std::endl;
#endif

			dbo::Transaction t(db_s);
			for(size_t i = 0; i < GenerationBatchSize; ++i)
			{
				dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));

				for(size_t j = params.pop_size * i; j < params.pop_size * (i + 1); ++j)
				{
					ind_data[j].generation = gen_ptr;
					db_s.add(new genome(ind_data[j]));
				}
			}
			t.commit();

#ifndef _DEBUG
			std::cout << "...committed." << std::endl;
#endif

			WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

			WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, obs_model, out.str()));

			gen_batch_index = 0;
			ind_batch_index = 0;
		}
	}

	if(gen_batch_index > 0)
	{
#ifndef _DEBUG
		std::cout << "Batching new generation records..." << std::endl;
#endif

		dbo::Transaction t(db_s);
		for(size_t i = 0; i < gen_batch_index; ++i)
		{
			dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));

			for(size_t j = params.pop_size * i; j < params.pop_size * (i + 1); ++j)
			{
				ind_data[j].generation = gen_ptr;
				db_s.add(new genome(ind_data[j]));
			}
		}
		t.commit();

#ifndef _DEBUG
		std::cout << "...committed." << std::endl;
#endif

		WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

		WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, obs_model, out.str()));
	}

	temp_ugly_hack< typename sim_t::scenario_t >::do_it(app, best);
	
	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::completion_cb, this, out.str()));
}
*/


#endif

