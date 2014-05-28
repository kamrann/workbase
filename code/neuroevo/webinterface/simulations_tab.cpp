// simulations_tab.cpp

#include "simulations_tab.h"
#include "webinterfaceapp.h"
#include "wt_system_widgets/properties_chart_widget.h"
#include "generic_system_coordinator.h"
#include "evo_db/evo_db.h"

#include "rtp_interface/rtp_sim.h"
#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/rtp_param_widget.h"
#include "rtp_interface/params/paramlist_par.h"
#include "rtp_interface/systems/nac/rtp_nac_scenario.h"
#include "rtp_interface/systems/sat/rtp_sat_scenario.h"

#include "evorun_params.h"
#include "system_params.h"
#include "phys_system_params.h"
#include "phys_agent_params.h"
#include "phys_spaceship_params.h"
#include "phys_controller_params.h"
#include "phys_mlp_controller_params.h"
#include "genalg_params.h"

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
// temp
#include <Wt/WTabWidget>
//

#include <boost/random/mersenne_twister.hpp>


///////////////////// TODO: Mega hack!!!!!!!!!!!!!!!
std::set< thruster_base::thruster_activation > opposing_thruster_activation_ofd::null_subsets;
///////////////////

SimulationsTab::SimulationsTab(WContainerWidget* parent):
	WContainerWidget(parent)
{
/*	WPanel* sys_params_panel = new WPanel(this);
	sys_params_panel->setTitle("System parameters");

	rtp_param_type* spt = i_system::params(true);
	system_params_widget = spt->create_widget(&param_mgr);
	sys_params_panel->setCentralWidget(system_params_widget->get_wt_widget());

	WPanel* sim_params_panel = new WPanel(this);
	sim_params_panel->setTitle("Simulation parameters");

	rtp_named_param_list evo_param_list = rtp_simulation::evo_params();
	rtp_param_type* evo_pt = new rtp_staticparamlist_param_type(evo_param_list);
	evo_params_widget = evo_pt->create_widget(&param_mgr);
	sim_params_panel->setCentralWidget(evo_params_widget->get_wt_widget());
*/

	params_wgt = db_hierarchy_level< evorun_params_defn >::create_widget(param_mgr);
	addWidget(params_wgt);

	run_sim_btn = new WPushButton("Run Simulation", this);              // create a button
	run_sim_btn->setFocus();
	run_sim_btn->setMargin(5, Left);                            // add 5 pixels margin

	addWidget(new WBreak());                       // insert a line break

	txt_output = new WTextArea(this);                         // empty text
	txt_output->resize(600, 250);
	txt_output->setReadOnly(true);

	observations_table = new WTableView(this);
	observations_table->setMargin(10, Top | Bottom);
	observations_table->setMargin(WLength::Auto, Left | Right);
	//period_table->setModel(period_q_model);
	observations_table->setSortingEnabled(false);
	observations_table->setMaximumSize(WLength::Auto, 300);
	observations_table->setSelectionMode(Wt::SingleSelection);

	evo_chart = new properties_chart_widget();
	rtp_stored_properties* props(new rtp_stored_properties());
	props->add_property("Generation");
	props->add_property("Highest Obj");
	props->add_property("Average Obj");
	props->add_property("Diversity");
	evo_chart->reset(boost::shared_ptr< i_properties const >(props));
	addWidget(evo_chart);

	run_sim_btn->clicked().connect(this, &SimulationsTab::on_run_simulation);
}

void SimulationsTab::on_run_simulation()
{
	evo_chart->clear_content();
	/*
	rtp_param sys_param = system_params_widget->get_param();
	rtp_param evo_param = evo_params_widget->get_param();
	rtp_simulation* sim = new rtp_simulation(sys_param, evo_param);

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	boost::function< void() > thread_func = boost::bind< void >(boost::mem_fn(&SimulationsTab::run_simulation_threadmain), this, app, sim);
	sim_thread.swap(boost::shared_ptr< boost::thread >(new boost::thread(thread_func)));
	run_sim_btn->setEnabled(false);
	*/
#if 0
	rtp_param sys_param = system_params_widget->get_param();

	sim_params params;
	params.pop_size = WLocale::currentLocale().toInt(pop_size_edit->text());
	params.num_epochs = WLocale::currentLocale().toInt(num_epochs_edit->text());
	params.trials_per_epoch = WLocale::currentLocale().toInt(trials_per_epoch_edit->text());

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	//boost::function< void(WebInterfaceApplication*, sim_params) > thread_func;
	boost::function< void() > thread_func;

	SystemType sys_type = boost::any_cast<SystemType>(boost::any_cast< std::pair< rtp_param, rtp_param > >(sys_param).first);
	switch(sys_type)
	{
	case SystemType::NoughtsAndCrosses:
	{
		typedef play_to_completion< 2, 3, true, boost::random::mt19937 > scenario_t;
		typedef scenario_t::system_t system_t;
		typedef nac::default_fixednn_solution< system_t, scenario_t::solution_input > solution_t;
		typedef fixed_neural_net< system_t, solution_t > genetic_mapping_t;
		struct trial_data_t
		{
			scenario_t::state initial_st;
			scenario_t::state final_st;

			// TODO: Dependent on template params as to whether is needed...????
			//EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		};
		typedef single_objective< trial_data_t, try_to_win_obj_fn > resultant_obj_fn_t;
		typedef basic_observer observer_t;

		typedef simulation<
			scenario_t,
			genetic_mapping_t,
			trial_data_t,
			resultant_obj_fn_t,
			coll_obj_val_identity< resultant_obj_fn_t::obj_value_t >,
			observer_t
		> sim_t;

		thread_func = boost::bind< void >(boost::mem_fn(&SimulationsTab::run_simulation_threadmain< sim_t >), this, app, params);
	}
	break;

	case ShipAndThrusters2D:
	{
		rtp_param_list sys_specific_params = boost::any_cast<rtp_param_list>(boost::any_cast<std::pair< rtp_param, rtp_param >>(sys_param).second);
		// First param is dimensionality
		// TODO:
		// Next is scenario
		std::pair< rtp_param, rtp_param > scenario_param = boost::any_cast<std::pair< rtp_param, rtp_param >>(sys_specific_params[1]);
		rtp_sat::sat_scenario::ScenarioType scenario = boost::any_cast<rtp_sat::sat_scenario::ScenarioType>(scenario_param.first);

		switch(scenario)
		{
		case rtp_sat::sat_scenario::ScenarioType::FullStop:
		{
			WorldDimensionality const dim = WorldDimensionality::dim2D;
			typedef full_stop< dim > scenario_t;
			typedef scenario_t::system_t system_t;
			typedef sat::full_stop_fixednn_solution< system_t, scenario_t::agent_sensor_state > solution_t;
			typedef fixed_neural_net< system_t, solution_t > genetic_mapping_t;
			struct trial_data_t
			{
				scenario_t::state initial_st;
				scenario_t::state final_st;

				// TODO: Dependent on template params as to whether is needed...????
				EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			};

			typedef single_objective< trial_data_t,
				//min_ang_speed_obj_fn >
				//min_avg_ang_speed_obj_fn >
				//min_lin_speed_obj_fn >
				//min_orient_delta_obj_fn >
				min_kinetic_obj_fn >
				//min_ke_and_opp_thrusters_obj_fn >

				/*		typedef pareto_multiple_objective< trial_data_t,
							boost::mpl::vector< min_kinetic_obj_fn, min_opp_thrusters_obj_fn > >
							*/			resultant_obj_fn_t;

			typedef sat_basic_observer observer_t;

			typedef simulation<
				scenario_t,
				genetic_mapping_t,
				trial_data_t,
				resultant_obj_fn_t,
				//			coll_obj_val_pareto_ranking< resultant_obj_fn_t::obj_value_t >,
				coll_obj_val_identity< resultant_obj_fn_t::obj_value_t >,
				observer_t
			> sim_t;

			thread_func = boost::bind< void >(boost::mem_fn(&SimulationsTab::run_simulation_threadmain< sim_t >), this, app, params);
		}
		break;

		case rtp_sat::sat_scenario::ScenarioType::AngularFullStop:
		{
			WorldDimensionality const dim = WorldDimensionality::dim2D;
			typedef angular_full_stop< dim > scenario_t;
			typedef scenario_t::system_t system_t;
			typedef sat::angular_full_stop_fixednn_solution< system_t, scenario_t::agent_sensor_state > solution_t;
			typedef fixed_neural_net< system_t, solution_t > genetic_mapping_t;
			struct trial_data_t
			{
				scenario_t::state initial_st;
				scenario_t::state final_st;

				// TODO: Dependent on template params as to whether is needed...????
				EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			};

			typedef single_objective< trial_data_t,
				//min_ang_speed_obj_fn >
				//min_avg_ang_speed_obj_fn >
				//min_lin_speed_obj_fn >
				//min_orient_delta_obj_fn >
				//min_kinetic_obj_fn >
				min_ke_and_opp_thrusters_obj_fn >

/*			typedef pareto_multiple_objective< trial_data_t,
				boost::mpl::vector< min_avg_ang_speed_obj_fn, min_opp_thrusters_obj_fn > >
*/				
				resultant_obj_fn_t;

			typedef sat_basic_observer observer_t;

			typedef simulation<
				scenario_t,
				genetic_mapping_t,
				trial_data_t,
				resultant_obj_fn_t,
				//coll_obj_val_pareto_ranking< resultant_obj_fn_t::obj_value_t >,
				coll_obj_val_identity< resultant_obj_fn_t::obj_value_t >,
				observer_t
			> sim_t;

			thread_func = boost::bind< void >(boost::mem_fn(&SimulationsTab::run_simulation_threadmain< sim_t >), this, app, params);
		}
		break;

		case rtp_sat::sat_scenario::ScenarioType::TargetOrientation:
		{
			WorldDimensionality const dim = WorldDimensionality::dim2D;
			typedef target_orientation< dim > scenario_t;
			typedef scenario_t::system_t system_t;
			typedef sat::target_orientation_fixednn_solution< system_t, scenario_t::agent_sensor_state > solution_t;
			typedef fixed_neural_net< system_t, solution_t > genetic_mapping_t;
			struct trial_data_t
			{
				scenario_t::state initial_st;
				scenario_t::state final_st;

				// TODO: Dependent on template params as to whether is needed...????
				EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			};

			typedef single_objective< trial_data_t,
				min_orient_delta_obj_fn >

				/*			typedef pareto_multiple_objective< trial_data_t,
				boost::mpl::vector< min_avg_ang_speed_obj_fn, min_opp_thrusters_obj_fn > >
				*/
				resultant_obj_fn_t;

			typedef sat_basic_observer observer_t;

			typedef simulation<
				scenario_t,
				genetic_mapping_t,
				trial_data_t,
				resultant_obj_fn_t,
				//coll_obj_val_pareto_ranking< resultant_obj_fn_t::obj_value_t >,
				coll_obj_val_identity< resultant_obj_fn_t::obj_value_t >,
				observer_t
			> sim_t;

			thread_func = boost::bind< void >(boost::mem_fn(&SimulationsTab::run_simulation_threadmain< sim_t >), this, app, params);
		}
		break;
		}
	}
	break;
	}

	sim_thread.swap(boost::shared_ptr< boost::thread >(new boost::thread(thread_func)));
	run_sim_btn->setEnabled(false);

#endif
}

// TODO: TEMP
#include "system_test_tab.h"
//

void SimulationsTab::run_simulation_threadmain(WebInterfaceApplication* app, rtp_simulation* sim)
{
	bool const RandomlySeeded = false;

	std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
	evodb_session db_s(*app->db_cp);

	std::stringstream out;
	out.precision(4);
	std::fixed(out);

	out << ("Initialising...") << std::endl;

	// Add database entry to evo_run table
	// Temp
	dbo::ptr< evo_run > ep;
/*	dbo::Transaction t(db_s);
	dbo::ptr< evo_run > ep = db_s.add(new evo_run);
//	ep.modify()->initial_pop_size = sim->population_size;//params.pop_size;
	t.commit();
*/
	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::evo_started_cb, this, ep));

	sim->init();

//	sim_t::observation_data_t observations;

	out << ("Training...") << std::endl;

	size_t const GenerationBatchSize =
#ifdef _DEBUG
		5
#else
		50
#endif
		;
	std::vector< generation > gen_data(GenerationBatchSize);
	std::vector< genome > ind_data(sim->population_size * GenerationBatchSize);
	size_t gen_batch_index = 0;
	size_t ind_batch_index = 0;

	std::vector< boost::shared_ptr< i_property_values const > > per_generation_prop_vals(GenerationBatchSize);

	// TODO: Assuming double
//	sim_t::processed_obj_val_t highest_ov;
//	sim_t::agent_t best;
	for(size_t e = 0; e < sim->total_generations; ++e)
	{
//		observations.clear();	// TODO:
		sim->run_epoch(/*observations,*/ out);

		boost::any average_obj = sim->get_average_objective();
		boost::any highest_obj = sim->get_highest_objective();
		genotype_diversity_measure diversity = sim->population_genotype_diversity();

		rtp_stored_property_values* vals = new rtp_stored_property_values();
		vals->set_value("Generation", boost::any(e + 1));
		vals->set_value("Average Obj", average_obj);
		vals->set_value("Highest Obj", highest_obj);
		vals->set_value("Diversity", diversity);
		per_generation_prop_vals[gen_batch_index].reset(vals);

/*		std::stringstream ss;
		ss.precision(2);
		std::fixed(ss);
		ss << "Gen " << (e + 1) << ": Average Obj. = " << boost::any_cast<double>(average_obj);
		ss << "; Highest = " << boost::any_cast<double>(highest_obj) << "." << std::endl;
		out << ss.str();
*/
		// Add database entries
		gen_data[gen_batch_index].index = sim->generation;
		gen_data[gen_batch_index].genotype_diversity = 0;// sim.population_genotype_diversity();
		gen_data[gen_batch_index].run = ep;
		++gen_batch_index;

//		std::sort(sim.ga.population.begin(), sim.ga.population.end());
		for(size_t i = 0; i < sim->population_size; ++i)
		{
			size_t rank = sim->population_size - i - 1;
			ind_data[ind_batch_index].gen_obj_rank = rank;
			// TODO: ind_data[ind_batch_index].encoding = sim.ga.population[rank].genome;
			++ind_batch_index;
		}

		if(gen_batch_index == GenerationBatchSize)
		{
#ifndef _DEBUG
			std::cout << "Batching new generation records..." << std::endl;
#endif

/*			dbo::Transaction t(db_s);
			for(size_t i = 0; i < GenerationBatchSize; ++i)
			{
				dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));

				for(size_t j = sim->population_size * i; j < sim->population_size * (i + 1); ++j)
				{
					ind_data[j].generation = gen_ptr;
					db_s.add(new genome(ind_data[j]));
				}
			}
			t.commit();
*/
#ifndef _DEBUG
			std::cout << "...committed." << std::endl;
#endif

//			WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

			WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, per_generation_prop_vals, out.str()));

			gen_batch_index = 0;
			ind_batch_index = 0;
		}
	}

	if(gen_batch_index > 0)
	{
#ifndef _DEBUG
		std::cout << "Batching new generation records..." << std::endl;
#endif

/*		dbo::Transaction t(db_s);
		for(size_t i = 0; i < gen_batch_index; ++i)
		{
			dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));

			for(size_t j = sim->population_size * i; j < sim->population_size * (i + 1); ++j)
			{
				ind_data[j].generation = gen_ptr;
				db_s.add(new genome(ind_data[j]));
			}
		}
		t.commit();
*/
#ifndef _DEBUG
		std::cout << "...committed." << std::endl;
#endif

//		WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

		per_generation_prop_vals.resize(gen_batch_index);
		WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, per_generation_prop_vals, out.str()));
	}

	{
		Wt::WApplication::UpdateLock lock(app->getUpdateLock());
		typedef generic_sys_coordinator coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(sim->get_fittest());
		coordinator->restart();
	}

	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::completion_cb, this, out.str()));
}

void SimulationsTab::evo_started_cb(dbo::ptr< evo_run > ep)
{
	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

// TODO: enable		app->db_session.new_evo_period_signal().emit(ep);
}

void SimulationsTab::generation_cb(std::vector< boost::shared_ptr< i_property_values const > > per_gen_prop_vals, std::string txt)
{
/*	if(obs_model)
	{
		observations_table->setModel(obs_model);
	}
*/
	txt_output->setText(txt);

	for(size_t i = 0; i < per_gen_prop_vals.size(); ++i)
	{
		evo_chart->append_data(per_gen_prop_vals[i]);
	}

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

// TODO: enable		app->db_session.new_generations_signal().emit();

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



