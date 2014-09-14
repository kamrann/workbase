// simulations_tab.cpp

#include "simulations_tab.h"
#include "webinterfaceapp.h"
#include "wt_system_widgets/properties_chart_widget.h"
#include "generic_system_coordinator.h"
#include "evo_db/evo_db.h"

#include "rtp_interface/rtp_sim.h"
#include "rtp_interface/rtp_genome.h"
#include "rtp_interface/rtp_procreation_selection.h"
#include "rtp_interface/systems/rtp_system.h"

#include "observer_data_models.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/param_accessor.h"
#include "wt_param_widgets/schema_builder.h"

#include <yaml-cpp/yaml.h>

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

#include <ctime>


///////////////////// TODO: Mega hack!!!!!!!!!!!!!!!
std::set< thruster_base::thruster_activation > opposing_thruster_activation_ofd::null_subsets;
///////////////////

// TEMP
#include "rtp_interface/systems/phys/rtp_phys_system.h"
#include "rtp_interface/systems/phys/rtp_phys_scenario.h"
#include "rtp_interface/systems/phys/rtp_phys_agent_body_spec.h"
#include "rtp_interface/systems/phys/bodies/basic_spaceship.h"
#include "rtp_interface/systems/phys/bodies/test_body.h"
#include "rtp_interface/systems/phys/bodies/test_biped_body.h"
#include "rtp_interface/systems/phys/bodies/test_quadruped_body.h"
#include "rtp_interface/systems/phys/rtp_phys_controller.h"
#include "rtp_interface/rtp_evolvable_controller.h"
#include "rtp_interface/systems/mlp.h"
#include "rtp_interface/rtp_resultant_objective.h"
#include "rtp_interface/rtp_manual_objective.h"
//

// TODO: Move
namespace sb = prm::schema;

std::string update_evolvablecontrollers_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
{
	auto relative = std::string{ "evolved_container" };
	auto path = prefix + relative;

	path += std::string("evolved_list");
	path += std::string("evolved");

	(*provider)[path + std::string("name")] = [](prm::param_accessor param_vals)
	{
		/*			auto default_name = std::string("Temp");
		auto path = param_vals.get_current_path();
		if(path.size() > 0)
		{
		auto node = param_vals["spec_type"];
		auto spec_type = node[0].as< Type >();
		default_name = Names[spec_type];

		default_name += "[";
		auto agent_it = path.find_anywhere("agent_list");
		default_name += std::to_string(agent_it->index());
		default_name += "]";

		default_name += "[";
		auto inst_it = path.find_anywhere("instance_list");
		default_name += std::to_string(inst_it->index());
		default_name += "]";
		}
		*/
		auto s = sb::string("name");// , default_name);
		sb::label(s, "Name");
		sb::trigger(s, "evolvablecontroller_name_modified");
		return s;
	};

	(*provider)[path + std::string("compatible_agent_types")] = [=](prm::param_accessor param_vals)
	{
		auto sys_type_node = param_vals["sys_type"];
		auto sys_type = sys_type_node[0].as< rtp::SystemType >();

		auto agent_types = rtp::i_system::get_agent_type_names(sys_type);

		auto s = sb::enum_selection(
			"compatible_agent_types",
			agent_types,
			0,
			// TODO: Allowing multi selection requires taking union of available state values across all selected
			// instance types for both controller inputs and objective values.
			// Also potentially need to provide for min/max/averaging over all instances sharing a controller for 
			// objective values, and option to tie objective value to just state values of a single instance.
			// Then of course, need to implement multi controller evolution within the system simulation.
			//num_available_instances
			1
			);
		sb::label(s, "Compatible Agents");
		sb::trigger(s, "evolved_controller_compatibility_changed");

		return s;
	};

	auto evolvable_rel = rtp::evolvable_controller::update_schema_providor(provider, path);
	auto objective_rel = rtp::resultant_objective::update_schema_providor(provider, path);

	(*provider)[path] = [=](prm::param_accessor param_vals)
	{
		auto s = sb::list("evolved");

		sb::append(s, provider->at(path + std::string("name"))(param_vals));
		sb::append(s, provider->at(path + std::string("compatible_agent_types"))(param_vals));
		sb::append(s, provider->at(path + evolvable_rel)(param_vals));
		sb::append(s, provider->at(path + objective_rel)(param_vals));

		sb::enable_import(s, "evolved_controller");
		return s;
	};

	path.pop();

	(*provider)[path] = [=](prm::param_accessor param_vals)
	{
		auto s = sb::repeating_list("evolved_list", std::string("evolved"), 0, 0);
//		sb::label(s, "Evolved Controllers");
		sb::enable_import(s, "evolved_controller_list");
		// TODO: tie to specific events
		sb::trigger(s, "evolved_controller_added");
		sb::trigger(s, "evolved_controller_removed");
		return s;
	};

	path.pop();

	(*provider)[path] = [=](prm::param_accessor param_vals)
	{
		auto s = sb::list("evolved_container");

		auto sys_type_node = param_vals["sys_type"];
		if(sys_type_node && !sys_type_node.as< prm::is_unspecified >())
		{
			sb::append(s, provider->at(path + std::string("evolved_list"))(param_vals));
		}
		else
		{
			// TODO: Message?
		}

		sb::label(s, "Evolved Controllers");
		sb::update_on(s, "sys_type_changed");	// TODO: not allowed for repeat??
		return s;
	};

	return relative;
}
//

SimulationsTab::SimulationsTab(WContainerWidget* parent):
	WContainerWidget(parent)
{
	auto spm = std::make_shared< prm::schema::schema_provider_map >();
	prm::qualified_path abs_path;
	auto root = std::string{ "sim_params" };
	abs_path += root;
	auto system_rel = rtp::i_system::update_schema_providor(spm, abs_path, true);
	auto evolved_rel = update_evolvablecontrollers_schema_providor(spm, abs_path);
	(*spm)[abs_path] = [=](prm::param_accessor param_vals)
	{
		auto s = sb::list("sim_params");
		sb::append(s, spm->at(abs_path + system_rel)(param_vals));
		sb::append(s, spm->at(abs_path + evolved_rel)(param_vals));
		sb::border(s, std::string{ "Simulation Params" });
		sb::layout_horizontal(s);
		sb::enable_import(s, "sim");
		return s;
	};
	(*spm)[prm::qualified_path()] = spm->at(root);	// TODO: Don't like this, but param_tree::create()
		// has no way to know name of root schema when it first requests the schema.
	system_params_tree = prm::param_tree::create(
		[spm](prm::qualified_path const& path, prm::param_accessor acc)
		{
			return spm->at(path.unindexed())(acc);
		});
	addWidget(system_params_tree);

	spm = std::make_shared< prm::schema::schema_provider_map >();
	abs_path = prm::qualified_path();
	root = rtp::rtp_simulation::update_schema_providor(spm, abs_path);
	(*spm)[prm::qualified_path()] = spm->at(root);

	evo_params_tree = prm::param_tree::create(
		[spm](prm::qualified_path const& path, prm::param_accessor acc)
	{
		return spm->at(path)(acc);
	});
	addWidget(evo_params_tree);

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
	auto props = std::make_shared< rtp::rtp_stored_properties >();
	props->add_property("Generation");
	props->add_property("Highest Obj");
	props->add_property("Average Obj");
	props->add_property("Diversity");
	evo_chart->reset(props);
	addWidget(evo_chart);

	run_sim_btn->clicked().connect(this, &SimulationsTab::on_run_simulation);
}

SimulationsTab::~SimulationsTab()
{

}

void SimulationsTab::on_run_simulation()
{
	evo_chart->clear_content();
	
	auto sys_param = system_params_tree->get_param_accessor();
	sys_param.push_relative_path(std::string{ "sim_params" });
	auto evo_param = evo_params_tree->get_param_accessor();

	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

	sim_thread = std::make_unique< std::thread >([=]{
		run_simulation_threadmain(app, sys_param, evo_param);
	});
	run_sim_btn->setEnabled(false);
	
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

void SimulationsTab::run_simulation_threadmain(WebInterfaceApplication* app,
	prm::param_accessor sys_param,
	prm::param_accessor evo_param)
{
	bool const RandomlySeeded = false;
	bool const SaveToDatabaseEnabled = evo_param["save_to_db"].as< bool >();

	std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
	evodb_session db_s(*app->db_cp);

	std::stringstream out;
	out.precision(4);
	std::fixed(out);

	out << ("Initialising...") << std::endl;

	dbo::ptr< evo_run > ep;
	if(SaveToDatabaseEnabled)
	{
		// Add database entry to evo_run table
		dbo::Transaction t(db_s);
		ep = db_s.add(new evo_run);
		//		ep.modify()->initial_pop_size = sim->population_size;//params.pop_size;
		ep.modify()->sys_params = YAML::Dump(sys_param.get_root());	// TODO: ????????
		ep.modify()->evo_params = YAML::Dump(evo_param.get_root());
		ep.modify()->started = std::chrono::system_clock::now();
		t.commit();
	}

	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::evo_started_cb, this, ep));

	auto sim = std::make_unique< rtp::rtp_simulation >(sys_param, evo_param);
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

	///
	auto on_generation_batch = [&sim, &db_s, &ep, &gen_data, &ind_data, &sys_param](size_t gen_count)
	{
#ifndef _DEBUG
		std::cout << "Batching new generation records..." << std::endl;
#endif

		dbo::Transaction t(db_s);
		for(size_t i = 0; i < gen_count; ++i)
		{
			dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));
			bool const last_gen = gen_ptr->index == sim->total_generations;

			for(size_t j = sim->population_size * i; j < sim->population_size * (i + 1); ++j)
			{
				ind_data[j].generation = gen_ptr;
				auto gn_ptr = db_s.add(new genome(ind_data[j]));

				bool const top_ranked = ind_data[j].gen_obj_rank == 0;
				if(last_gen && top_ranked)
				{
					dbo::ptr< named_genome > named = new named_genome;
					named.modify()->gn = gn_ptr;
					std::stringstream name;
					name << rtp::SystemNames[sys_param["sys_type"][0].as< rtp::SystemType >()];
					name << " ";
					auto starttime = std::chrono::system_clock::to_time_t(ep->started);
					std::tm tm;
					::localtime_s(&tm, &starttime);
					// TODO: Make 100% certain we are not overwriting a record with the exact same name
					name << std::put_time(&tm, "%d/%m/%y - %H:%M:%S");
					if(true)	// TODO: if multiple top ranked
					{
						name << " [" << (j % sim->population_size + 1) << "]";
					}
					named.modify()->name = name.str();
					db_s.add(named);
				}
			}
		}
		t.commit();

#ifndef _DEBUG
		std::cout << "...committed." << std::endl;
#endif
	};
	///

	size_t gen_batch_index = 0;
	size_t ind_batch_index = 0;

	std::vector< std::shared_ptr< rtp::i_property_values const > > per_generation_prop_vals(GenerationBatchSize);

	// TODO: Assuming double
//	sim_t::processed_obj_val_t highest_ov;
//	sim_t::agent_t best;
	for(size_t e = 0; e < sim->total_generations; ++e)
	{
//		observations.clear();	// TODO:
		sim->run_epoch(/*observations,*/ out);

		boost::any average_obj = sim->get_average_objective();
		boost::any highest_obj = sim->get_highest_objective();
		rtp::genotype_diversity_measure diversity = sim->population_genotype_diversity();

		auto vals = std::make_shared< rtp::rtp_stored_property_values >();
		vals->set_value("Generation", boost::any(e + 1));
		vals->set_value("Average Obj", average_obj);
		vals->set_value("Highest Obj", highest_obj);
		vals->set_value("Diversity", diversity);
		per_generation_prop_vals[gen_batch_index] = vals;

		if(SaveToDatabaseEnabled)
		{
			// Add database entries
			gen_data[gen_batch_index].index = sim->generation;
			gen_data[gen_batch_index].genotype_diversity = 0;// sim.population_genotype_diversity();
			gen_data[gen_batch_index].run = ep;
		}
		++gen_batch_index;	// TODO: currently even if not saving to db, batch counts are still used 
			// to determine when to invoke callbacks. this should probably be separated.

		struct idv_sortdata {
			size_t idx;
			double fitness;
		};

		std::vector< idv_sortdata > sort_data(sim->population_size, idv_sortdata());
		for(size_t i = 0; i < sim->population_size; ++i)
		{
			sort_data[i].idx = i;
			sort_data[i].fitness = sim->population[i].obj_value;//sim->population[i].fitness;
		}
		std::sort(begin(sort_data), end(sort_data), [](idv_sortdata const& lhs, idv_sortdata const& rhs)
		{
			return lhs.fitness > rhs.fitness;
		});

		size_t rank = 0;
		if(SaveToDatabaseEnabled)
		{
			for(size_t i = 0; i < sim->population_size; ++i)
			{
				if(i > 0 && sort_data[i].fitness < sort_data[i - 1].fitness)
				{
					rank = i;
				}

				ind_data[ind_batch_index].gen_obj_rank = rank;
				ind_data[ind_batch_index].encoding.clear();	// Needed since being reused and to_binary just appends values
				sim->population[sort_data[i].idx].gn->to_binary(ind_data[ind_batch_index].encoding);
				++ind_batch_index;
			}
		}

		if(gen_batch_index == GenerationBatchSize)
		{
			if(SaveToDatabaseEnabled)
			{
				on_generation_batch(GenerationBatchSize);
			}
/*
#ifndef _DEBUG
			std::cout << "Batching new generation records..." << std::endl;
#endif

			dbo::Transaction t(db_s);
			for(size_t i = 0; i < GenerationBatchSize; ++i)
			{
				dbo::ptr< generation > gen_ptr = db_s.add(new generation(gen_data[i]));
				bool const last_gen = gen_ptr->index == sim->total_generations;

				for(size_t j = sim->population_size * i; j < sim->population_size * (i + 1); ++j)
				{
					ind_data[j].generation = gen_ptr;
					auto gn_ptr = db_s.add(new genome(ind_data[j]));

					bool const top_ranked = ind_data[j].gen_obj_rank == 0;
					if(last_gen && top_ranked)
					{
						dbo::ptr< named_genome > named = new named_genome;
						named.modify()->gn = gn_ptr;
						std::stringstream name;
						name << SystemNames[sys_param["sys_type"][0].as< SystemType >()];
						name << " ";
						auto starttime = std::chrono::system_clock::to_time_t(ep->started);
						std::tm tm;
						::localtime_s(&tm, &starttime);
						name << std::put_time(&tm, "%d/%m/%y - %H:%M");
						if(true)	// TODO: if multiple top ranked
						{
							name << " [" << (j % sim->population_size + 1) << "]";
						}
						named.modify()->name = name.str();
						db_s.add(named);
					}
				}
			}
			t.commit();

#ifndef _DEBUG
			std::cout << "...committed." << std::endl;
#endif
*/

			//			WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

			WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, per_generation_prop_vals, out.str()));

			gen_batch_index = 0;
			ind_batch_index = 0;
		}
	}

	if(gen_batch_index > 0)
	{
		if(SaveToDatabaseEnabled)
		{
			on_generation_batch(gen_batch_index);
		}
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

//		WStandardItemModel* obs_model = observer_data_model< sim_t::observer_t >::generate(observations);

		per_generation_prop_vals.resize(gen_batch_index);
		WServer::instance()->post(session_id, boost::bind(&SimulationsTab::generation_cb, this, per_generation_prop_vals, out.str()));
	}

	if(SaveToDatabaseEnabled)
	{
		dbo::Transaction t(db_s);
		ep.modify()->ended = std::chrono::system_clock::now();
		t.commit();
	}

/*	{
		Wt::WApplication::UpdateLock lock(app->getUpdateLock());
		typedef generic_sys_coordinator coordinator_t;
		coordinator_t* coordinator = (coordinator_t*)((SystemTestTab*)app->main_tabs->widget(2))->coordinator;
		coordinator->set_agent_controllers(sim->get_fittest());
		coordinator->restart();
	}
*/
	WServer::instance()->post(session_id, boost::bind(&SimulationsTab::completion_cb, this, out.str()));
}

void SimulationsTab::evo_started_cb(dbo::ptr< evo_run > ep)
{
	WebInterfaceApplication* app = (WebInterfaceApplication*)WApplication::instance();

// TODO: enable		app->db_session.new_evo_period_signal().emit(ep);
}

void SimulationsTab::generation_cb(std::vector< std::shared_ptr< rtp::i_property_values const > > per_gen_prop_vals, std::string txt)
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



