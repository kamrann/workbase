// rtp_sat_scenario.cpp

#include "rtp_sat_scenario.h"
#include "scenarios/full_stop/rtp_sat_full_stop.h"
#include "scenarios/target_orientation/rtp_sat_target_orientation.h"
#include "../../rtp_param.h"
#include "../../params/paramlist_par.h"
#include "../../params/integer_par.h"
#include "../../params/fixed_or_random_par.h"

#include <Wt/WComboBox>


namespace rtp_sat {

	const std::array< std::string, sat_scenario_base::NumScenarios > sat_scenario_base::ScenarioNames = {
//		"Angular Full Stop",
//		"Linear Full Stop",
		"Full Stop",
		"Target Orientation",
	};

	template < WorldDimensionality dim >
	sat_scenario< dim >::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < NumScenarios; ++i)
		{
			add_item(ScenarioNames[i], (ScenarioType)i);
		}
		set_default_index(0);
	}

	
	template < WorldDimensionality dim >
	size_t sat_scenario< dim >::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return 0;
	}

	template < WorldDimensionality dim >
	rtp_named_param sat_scenario< dim >::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		// TODO:
		return rtp_named_param();
	}

	template < WorldDimensionality dim >
	//rtp_named_param_list
	rtp_param_type* sat_scenario< dim >::params()
	{
		rtp_named_param_list p;
		p.push_back(rtp_named_param(new sat_scenario< dim >::enum_param_type(), "Scenario"));
		//p.push_back(rtp_named_param(new sat_scenario< dim >::param_type()));
		p.push_back(rtp_named_param(new rtp_fixed_or_random_param_type(0.0, -1.0, 1.0), "Initial Ang Vel"));
		return new rtp_staticparamlist_param_type(p);
	}

	template < WorldDimensionality dim >
	rtp_named_param_list sat_scenario< dim >::params(ScenarioType scen)
	{
		switch(scen)
		{
		case FullStop:
			return full_stop< dim >::params();
		case TargetOrientation:
			return target_orientation< dim >::params();
		default:
			assert(false);
			return rtp_named_param_list();
		}
	}

	template < WorldDimensionality dim >
	sat_scenario< dim >* sat_scenario< dim >::create_instance(rtp_param param)
	{
		auto param_list = boost::any_cast<rtp_param_list>(param);
		ScenarioType scenario_type = boost::any_cast<ScenarioType>(param_list[0]);
		sat_scenario< dim >* scenario = nullptr;
		switch(scenario_type)
		{
			case FullStop:
			scenario = new full_stop< dim >(rtp_param());
			break;
		
			case TargetOrientation:
			scenario = new target_orientation< dim >(rtp_param());
			break;
		
			default:
			assert(false);
		}

		auto init_angvel = boost::any_cast<fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t >>(param_list[1]);
		scenario->m_initial_ang_vel = init_angvel;
		return scenario;
	}

	template < WorldDimensionality dim >
	typename sat_scenario< dim >::scenario_data_t sat_scenario< dim >::get_scenario_data() const
	{
		return scenario_data_t();
	}

	template < WorldDimensionality dim >
	bool sat_scenario< dim >::is_complete(state_t const& st)
	{
		return st.time >= 10.0;	// TODO:
	}

#if 0
	template < WorldDimensionality dim >
	rtp_named_param_list angular_full_stop< dim >::params()
	{
		rtp_named_param_list p;
		return p;
	}

	template < WorldDimensionality dim >
	angular_full_stop< dim >::angular_full_stop(rtp_param param)
	{
		rtp_param_list& param_list = boost::any_cast<rtp_param_list&>(param);

	}

	template < WorldDimensionality dim >
	typename sat_system< dim >::state angular_full_stop< dim >::generate_initial_state(rgen_t& rgen) const
	{
		typedef DimensionalityTraits< dim > dim_traits_t;

		state_t st;

		double const InitialAngularSpeed = 0.5;
/*		if(Dim == WorldDimensionality::dim2D && trial_context)
		{
			// Ensure half of trials have -ve initial rotation and half have +ve
			st.agents[i].ang_velocity = (trial_context->first % 2 == 0) ? InitialAngularSpeed : -InitialAngularSpeed;
		}
		else
*/		{
			st.ship.ang_velocity = random_dir_val< typename dim_traits_t::angular_velocity_t >(rgen) * InitialAngularSpeed;
		}

		return st;
	}


	template < WorldDimensionality dim >
	rtp_named_param_list linear_full_stop< dim >::params()
	{
		rtp_named_param_list p;
		return p;
	}

	template < WorldDimensionality dim >
	linear_full_stop< dim >::linear_full_stop(rtp_param param)
	{
		rtp_param_list& param_list = boost::any_cast<rtp_param_list&>(param);

	}

	template < WorldDimensionality dim >
	typename sat_system< dim >::state linear_full_stop< dim >::generate_initial_state(rgen_t& rgen)
	{
		typedef DimensionalityTraits< dim > dim_traits_t;

		state_t st;
		st.ship.thruster_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(thruster_presets::square_minimal()));
		st.thrusters.cfg = st.ship.thruster_cfg;
		st.thrusters.sync_to_cfg();

		double const InitialLinearSpeed = 2.0;
		double const InitialAngularSpeed = 0.5;

		st.ship.lin_velocity = random_dir_val< typename dim_traits_t::velocity_t >(rgen) * InitialLinearSpeed;
		st.ship.ang_velocity = random_dir_val< typename dim_traits_t::angular_velocity_t >(rgen) * InitialAngularSpeed;

		return st;
	}
#endif


	template class sat_scenario< WorldDimensionality::dim2D >;
	//template class sat_scenario< WorldDimensionality::dim3D >;
}


