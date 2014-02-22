// simulations_tab.cpp

#include "simulations_tab.h"
#include "webinterfaceapp.h"
#include "evo_db/system_sim_tbl.h"
#include "evo_db/problem_domain_tbl.h"
#include "evo_db/genetic_language_tbl.h"
#include "evo_db/evo_period_tbl.h"
#include "evo_db/generation_tbl.h"

#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/rtp_param_widget.h"
#include "rtp_interface/systems/nac/rtp_nac_scenario.h"
#include "rtp_interface/systems/sat/rtp_sat_scenario.h"
#include "sim.h"

#include "systems/noughts_and_crosses/scenarios/play_to_completion.h"
#include "systems/noughts_and_crosses/objectives/try_to_win.h"
#include "systems/noughts_and_crosses/observers/basic_observer.h"
#include "systems/noughts_and_crosses/genetic_mappings/fixednn_sln.h"

#include "systems/ship_and_thrusters/scenarios/angular_full_stop.h"
#include "systems/ship_and_thrusters/scenarios/full_stop.h"
#include "systems/ship_and_thrusters/scenarios/target_orientation.h"
#include "systems/ship_and_thrusters/objectives/minimize_angular_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_linear_speed.h"
#include "systems/ship_and_thrusters/objectives/minimize_orientation_delta.h"
#include "systems/ship_and_thrusters/objectives/minimize_kinetic_energy.h"
#include "systems/ship_and_thrusters/objectives/minimize_opposing_thrusters.h"
#include "systems/ship_and_thrusters/objectives/minimize_ke_and_opp_thrusters.h"
#include "systems/ship_and_thrusters/observers/sat_basic_observer.h"
#include "systems/ship_and_thrusters/genetic_mappings/fixednn_sln.h"

#include "systems/fixed_neural_net.h"

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


///////////////////// TODO: Mega hack!!!!!!!!!!!!!!!
std::set< thruster_base::thruster_activation > opposing_thruster_activation_ofd::null_subsets;
///////////////////

SimulationsTab::SimulationsTab(WContainerWidget* parent):
	WContainerWidget(parent)
{
	WPanel* sys_params_panel = new WPanel(this);
	sys_params_panel->setTitle("System parameters");

	system_param_type* spt = new system_param_type;
	system_params_widget = spt->create_widget();
	//addWidget(system_params_widget->get_wt_widget());
	sys_params_panel->setCentralWidget(system_params_widget->get_wt_widget());

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

	sim_params_table->elementAt(row, 0)->addWidget(new WText("Trials/Generation"));
	trials_per_epoch_edit = new WLineEdit(sim_params_table->elementAt(row, 1));
	trials_per_epoch_edit->setValidator(new Wt::WIntValidator(1, std::numeric_limits< int >::max()));
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



