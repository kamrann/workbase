/*
* Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
*
* See the LICENSE file for terms of use.
*/

#include "sim.h"
#include "observer_data_models.h"

#include "systems/noughts_and_crosses/scenarios/play_to_completion.h"

#include "agent_test_display.h"
#include "evo_db/evodb_session.h"
#include "evo_db/system_sim_tbl.h"
#include "evo_db/problem_domain_tbl.h"
#include "evo_db/genetic_language_tbl.h"
#include "evo_db/evo_period_tbl.h"
#include "evo_db/generation_tbl.h"
#include "evo_db/genome_tbl.h"

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WTableView>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WIntValidator>
#include <Wt/WItemDelegate>

#include <Wt/Dbo/QueryModel>

#include <doublefann.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/random/mersenne_twister.hpp>

// c++0x only, for std::bind
// #include <functional>

using namespace Wt;

/*
* A simple hello world application class which demonstrates how to react
* to events, read input, and give feed-back.
*/
class HelloApplication : public WApplication
{
public:
	HelloApplication(const WEnvironment& env, dbo::SqlConnectionPool& db);

private:
	void run_alg();
	void period_sel_changed();

public:
	void generation_cb(WStandardItemModel* obs_model, std::string txt);
	void completion_cb(std::string txt);

public:
	WLineEdit* pop_size_edit;
	WLineEdit* num_epochs_edit;
	
	WTextArea* output_;
	WTableView* observations_table;

	dbo::SqlConnectionPool* db_cp;
	evodb_session db_session;
	dbo::Query< dbo::ptr< evo_period > > period_query;
	dbo::Query< dbo::ptr< generation > > generation_query;
	//dbo::Query< dbo::ptr< genotype > > genotype_query;
	WTableView* period_table;
	WTableView* generation_table;
	//WTableView* individual_table;

	boost::shared_ptr< boost::thread > ga_thread;
};

/*
* The env argument contains information about the new session, and
* the initial request. It must be passed to the WApplication
* constructor so it is typically also an argument for your custom
* application constructor.
*/
HelloApplication::HelloApplication(const WEnvironment& env, dbo::SqlConnectionPool& db):
	WApplication(env),
	db_cp(&db),
	db_session(db)
{
	setTitle("GA Test");                               // application title

	


	

	//useStyleSheet("css/html4_default.css");

//	agent_test_display* glw = new agent_test_display(root(), false);
//	glw->resize(400, 300);
}

void HelloApplication::generation_cb(WStandardItemModel* obs_model, std::string txt)
{
	observations_table->setModel(obs_model);

	output_->setText(txt);

	dbo::QueryModel< dbo::ptr< evo_period > >* period_mod = (dbo::QueryModel< dbo::ptr< evo_period > >*)period_table->model();
	period_mod->reload();
	dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
	generation_mod->reload();

	triggerUpdate();
}

void HelloApplication::completion_cb(std::string txt)
{
//	HelloApplication* inst = (HelloApplication*)WApplication::instance();

	output_->setText(txt);
	triggerUpdate();
	enableUpdates(false);

	ga_thread->join();
	ga_thread.reset();
}

struct sim_params
{
	size_t pop_size;
	size_t num_epochs;
};

void run_fn(WApplication* app, dbo::SqlConnectionPool* db_cp, sim_params params)
{
//	WorldDimensionality const dim = WorldDimensionality::dim2D;
//	typedef simulation< angular_full_stop< dim > > sim_t;
	typedef simulation< play_to_completion< 2, 3, boost::random::mt19937 > > sim_t;

	evodb_session db_s(*db_cp);

	std::stringstream out;
	out.precision(4);
	std::fixed(out);

	out << ("Initialising...") << std::endl;

	// Add database entry to evo_period table
	dbo::Transaction t(db_s);
	dbo::ptr< evo_period > ep = db_s.add(new evo_period);
	ep.modify()->initial_pop_size = params.pop_size;
	t.commit();

	sim_t sim(
		params.pop_size,
		10
		);
	sim.init();

	out << ("Training...") << std::endl;

	size_t const GenerationBatchSize = 50;
	std::vector< generation > gen_data(GenerationBatchSize);
	size_t gen_batch_index = 0;

	sim_t::processed_obj_val_t highest_ov(0.0);
	//FANN::neural_net overall_best;
	for(int e = 0; e < params.num_epochs; ++e)
	{
		sim_t::observation_data_t observations;

		sim.run_epoch(observations, out);

/*		FANN::neural_net best;
		sim_t::processed_obj_val_t obj_value = sim.get_fittest(best);

		if (
//			!fitness.unfit && highest_fitness.unfit || fitness.unfit == highest_fitness.unfit &&
			obj_value > highest_ov || true)	// TODO: true for Pareto
		{
			highest_ov = obj_value;
			overall_best = best;
		}
*/
		//		cout << ("Epoch ") << (e + 1) << (": Average = ") << sim.ga.average_fitness << (", Best = ") << sim.ga.best_fitness << endl;

		// Add database entries
		gen_data[gen_batch_index].index = sim.ga.generation;
		gen_data[gen_batch_index].genotype_diversity = sim.population_genotype_diversity();
		gen_data[gen_batch_index].period = ep;
		++gen_batch_index;
		if(gen_batch_index == GenerationBatchSize)
		{
			dbo::Transaction t(db_s);
			for(size_t i = 0; i < GenerationBatchSize; ++i)
			{
				db_s.add(new generation(gen_data[i]));
			}
			t.commit();

			gen_batch_index = 0;
		}

		WStandardItemModel* obs_model = observer_data_model< basic_observer >::generate(observations);

		std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
		WServer::instance()->post(session_id, boost::bind(&HelloApplication::generation_cb, (HelloApplication*)app, obs_model, out.str()));
	}

//	out << "Training completed. Best fitness: " << highest_ov << std::endl;

	std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
	WServer::instance()->post(session_id, boost::bind(&HelloApplication::completion_cb, (HelloApplication*)app, out.str()));
}

void HelloApplication::run_alg()
{
	sim_params params;
	params.pop_size = WLocale::currentLocale().toInt(pop_size_edit->text());
	params.num_epochs = WLocale::currentLocale().toInt(num_epochs_edit->text());

	enableUpdates(true);	// TODO: Right place? And where to disable?
	ga_thread.swap(boost::shared_ptr< boost::thread >(new boost::thread(run_fn, this, db_cp, params)));//sessionId())));
}

void HelloApplication::period_sel_changed()
{
	generation_query.reset();
	auto sel = period_table->selectedIndexes();
	if (!sel.empty())
	{
		dbo::QueryModel< dbo::ptr< evo_period > >* period_mod = (dbo::QueryModel< dbo::ptr< evo_period > >*)period_table->model();
		std::stringstream ss;
		ss << period_mod->resultRow(sel.begin()->row()).id();
		generation_query.bind(ss.str());
		dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
		generation_mod->setQuery(generation_query, true);
//		generation_mod->reload();
	}
}

WApplication* createApplication(const WEnvironment& env, dbo::SqlConnectionPool* db)
{
	/*
	* You could read information from the environment to decide whether
	* the user has permission to start a new application
	*/
	return new HelloApplication(env, *db);
}

int main(int argc, char **argv)
{
	/*
	* Your main method may set up some shared resources, but should then
	* start the server application (FastCGI or httpd) that starts listening
	* for requests, and handles all of the application life cycles.
	*
	* The last argument to WRun specifies the function that will instantiate
	* new application objects. That function is executed when a new user surfs
	* to the Wt application, and after the library has negotiated browser
	* support. The function should return a newly instantiated application
	* object.
	*/

	WServer server(argv[0]);
	server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

	Wt::Dbo::SqlConnectionPool* db = evodb_session::create_connection_pool(server.appRoot() + "evo.db");

	server.addEntryPoint(Application, boost::bind(&createApplication, _1, db), "/home");

	if(server.start())
	{
		WServer::waitForShutdown();
		server.stop();
	}

	return 0;

//	return WRun(argc, argv, &createApplication);
}
