// simulations_tab.cpp

#include "simulations_tab.h"
#include "webinterfaceapp.h"
#include "evo_db/system_sim_tbl.h"
#include "evo_db/problem_domain_tbl.h"
#include "evo_db/genetic_language_tbl.h"
#include "evo_db/evo_period_tbl.h"
#include "evo_db/generation_tbl.h"

// TEMP
#include "system_test_tab.h"
#include "nac_system_coordinator.h"
#include <Wt/WTabWidget>
//

#include "sim.h"

#include "systems/noughts_and_crosses/scenarios/play_to_completion.h"

#include "observer_data_models.h"

#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WTableView>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WIntValidator>
#include <Wt/WLocale>
#include <Wt/WServer>

#include <boost/random/mersenne_twister.hpp>


SimulationsTab::SimulationsTab(WContainerWidget* parent):
	WContainerWidget(parent)
{
	WPanel* sim_params_panel = new WPanel(this);
	sim_params_panel->setTitle("Simulation parameters");

	WTable* sim_params_table = new WTable;

	size_t row = 0;
	sim_params_table->elementAt(row, 0)->addWidget(new WText("Population Size"));
	pop_size_edit = new WLineEdit(sim_params_table->elementAt(row, 1));
	pop_size_edit->setValidator(new Wt::WIntValidator(1, std::numeric_limits< int >::max()));
	++row;

	sim_params_table->elementAt(row, 0)->addWidget(new WText("Num Generations"));
	num_epochs_edit = new WLineEdit(sim_params_table->elementAt(row, 1));
	num_epochs_edit->setValidator(new Wt::WIntValidator(1, std::numeric_limits< int >::max()));
	++row;

	sim_params_panel->setCentralWidget(sim_params_table);

	run_sim_btn = new WPushButton("Run Simulation", this);              // create a button
	run_sim_btn->setFocus();
	run_sim_btn->setMargin(5, Left);                            // add 5 pixels margin

	addWidget(new WBreak());                       // insert a line break

	txt_output = new WTextArea(this);                         // empty text
	txt_output->resize(400, 250);
	txt_output->setReadOnly(true);

	observations_table = new WTableView(this);
	observations_table->setMargin(10, Top | Bottom);
	observations_table->setMargin(WLength::Auto, Left | Right);
	//period_table->setModel(period_q_model);
	observations_table->setSortingEnabled(false);
	observations_table->setMaximumSize(WLength::Auto, 300);
	observations_table->setSelectionMode(Wt::SingleSelection);

	run_sim_btn->clicked().connect(this, &SimulationsTab::on_run_simulation);
}

void SimulationsTab::on_run_simulation()
{
	sim_params params;
	params.pop_size = WLocale::currentLocale().toInt(pop_size_edit->text());
	params.num_epochs = WLocale::currentLocale().toInt(num_epochs_edit->text());

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	sim_thread.swap(boost::shared_ptr< boost::thread >(new boost::thread(&SimulationsTab::run_simulation_threadmain, this, app, params)));

	run_sim_btn->setEnabled(false);
}

void SimulationsTab::evo_started_cb(dbo::ptr< evo_period > ep)
{
	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	app->db_session.new_evo_period_signal().emit(ep);
	//app->triggerUpdate();
}

void SimulationsTab::generation_cb(WStandardItemModel* obs_model, std::string txt)
{
	observations_table->setModel(obs_model);
	txt_output->setText(txt);

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	app->db_session.new_generations_signal().emit();

	app->triggerUpdate();
}

void SimulationsTab::completion_cb(std::string txt)
{
	txt_output->setText(txt);
	
	sim_thread->join();
	sim_thread.reset();

	run_sim_btn->setEnabled(true);

	WApplication::instance()->triggerUpdate();
}

void SimulationsTab::run_simulation_threadmain(WebInterfaceApplication* app, sim_params params)
{
	//	WorldDimensionality const dim = WorldDimensionality::dim2D;
	//	typedef simulation< angular_full_stop< dim > > sim_t;
	typedef simulation< play_to_completion< 2, 3, true, boost::random::mt19937 > > sim_t;

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
		10
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

	sim_t::processed_obj_val_t highest_ov(0.0);
	sim_t::agent_t best;
	for(size_t e = 0; e < params.num_epochs; ++e)
	{
		observations.clear();	// TODO:
		sim.run_epoch(observations, out);

		sim_t::agent_t best_of_generation;
		sim_t::processed_obj_val_t obj_val = sim.get_fittest(best_of_generation);
		if(obj_val > highest_ov)
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

			WStandardItemModel* obs_model = observer_data_model< basic_observer >::generate(observations);

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

		WStandardItemModel* obs_model = observer_data_model< basic_observer >::generate(observations);

		WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, obs_model, out.str()));
	}

	/////////
	fixed_neural_net<
		noughts_and_crosses_system< 2, 3 >,
		play_to_completion< 2, 3, false, boost::random::mt19937 >::solution_input
	>::solution best_copy;
	best_copy.nn = best.nn;
	typedef nac_coordinator< play_to_completion< 2, 3, false, boost::random::mt19937 > > coordinator_t;
	coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
	coordinator->set_agent_controllers(
		new coordinator_t::nn_controller(best_copy),
		new coordinator_t::ui_agent_controller()
		);
	/////////

	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::completion_cb, this, out.str()));
}


