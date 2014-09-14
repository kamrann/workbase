// agents.cpp

#include "agents.h"
#include "../../../rtp_interactive_input.h"
#include "../../../../params/paramlist_par.h"
#include "../../../../params/integer_par.h"
#include "../../../../rtp_fixednn_genome_mapping.h"

#include <Wt/WComboBox>


namespace rtp_sat
{
	template < WorldDimensionality dim >
	std::string const mlp_agent< dim >::Names[] = {
		"Angular Velocity",
		"Velocity & Angle",
		"Velocity & Angle & Pos",
	};

	template < WorldDimensionality dim >
	typename passive_agent< dim >::decision_t passive_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;
		size_t const num_thrusters = st.ship.thruster_cfg->num_thrusters();
		dec.resize(num_thrusters, false);
		return dec;
	}

	template < WorldDimensionality dim >
	interactive_agent< dim >::interactive_agent(): inputs(10)	// TODO: input size
	{}

	template < WorldDimensionality dim >
	typename interactive_agent< dim >::decision_t interactive_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;
		size_t const num_thrusters = st.ship.thruster_cfg->num_thrusters();
		dec.resize(num_thrusters);
		for(size_t i = 0; i < num_thrusters; ++i)
		{
			dec[i] = inputs[i];
		}
		return dec;
	}

	template < WorldDimensionality dim >
	void interactive_agent< dim >::register_input(interactive_input const& input)
	{
		if(input.key_index >= inputs.size())
		{
			inputs.resize(input.key_index + 1);
		}

		inputs[input.key_index] = input.down;
	}

	
	template < WorldDimensionality dim >
	mlp_agent< dim >::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	template < WorldDimensionality dim >
	size_t mlp_agent< dim >::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return 2;
	}

	template < WorldDimensionality dim >
	rtp_named_param mlp_agent< dim >::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		switch(index)
		{
			case 0:
			return rtp_named_param(new enum_param_type(), "Inputs");

			case 1:
			return rtp_named_param(new rtp_integer_param_type(3, 2, 5), "Num Layers");

			default:
			return rtp_named_param();
		}
	}

	template < WorldDimensionality dim >
	std::tuple< i_genome_mapping*, i_agent_factory* > mlp_agent< dim >::create_instance_evolvable(rtp_param param, thruster_config< dim > const& cfg)
	{
		size_t const NumNNOutputs = cfg.num_thrusters();

		auto param_list = boost::any_cast< rtp_param_list >(param);
		Type type = boost::any_cast<Type>(param_list[0]);
		int num_layers = boost::any_cast<int>(param_list[1]);

		switch(type)
		{
			case AngVel:
			{
				size_t const NumPerHidden = (mlp_ang_vel_agent< dim >::NumNNInputs + NumNNOutputs) / 2;
				return std::tuple< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
					//mlp_ang_vel_agent< dim >::NumNNLayers,
					num_layers,
					mlp_ang_vel_agent< dim >::NumNNInputs,
					NumNNOutputs,
					NumPerHidden),
					new mlp_agent_factory< dim >(type, num_layers, NumNNOutputs));
			}

			case VelAngle:
			{
				size_t const NumPerHidden = (mlp_vel_and_angle_agent< dim >::NumNNInputs + NumNNOutputs) / 2;
				return std::tuple< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
						//mlp_vel_and_angle_agent< dim >::NumNNLayers,
						num_layers,
						mlp_vel_and_angle_agent< dim >::NumNNInputs,
						NumNNOutputs,
						NumPerHidden),
					new mlp_agent_factory< dim >(type, num_layers, NumNNOutputs));
			}

			case VelAnglePos:
			{
				size_t const NumPerHidden = (mlp_vel_angle_pos_agent< dim >::NumNNInputs + NumNNOutputs) / 2;
				return std::tuple< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
						num_layers,
						mlp_vel_angle_pos_agent< dim >::NumNNInputs,
						NumNNOutputs,
						NumPerHidden),
					new mlp_agent_factory< dim >(type, num_layers, NumNNOutputs));
			}

		default:
			assert(false);
			return std::tuple< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

	template < WorldDimensionality dim >
	mlp_agent< dim >::mlp_agent(size_t num_layers, size_t num_outputs): num_nn_layers(num_layers), num_nn_outputs(num_outputs)
	{
		assert(num_nn_layers >= 2);
		assert(num_nn_outputs >= 1);
	}

	template < WorldDimensionality dim >
	typename mlp_agent< dim >::decision_t mlp_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;

		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);

		size_t const num_thrusters = st.ship.thruster_cfg->num_thrusters();
		dec.resize(num_thrusters);
		for(size_t i = 0; i < num_thrusters; ++i)
		{
			dec[i] = nn_outputs[i] > 0.0;
		}
		return dec;
	}

	template < WorldDimensionality dim >
	mlp_ang_vel_agent< dim >::mlp_ang_vel_agent(size_t num_layers, size_t num_outputs): mlp_agent< dim >(num_layers, num_outputs)
	{
		size_t const NumNNHidden = (NumNNInputs + num_nn_outputs) / 2;

		std::vector< unsigned int > layer_neurons(num_nn_layers, (unsigned int)NumNNHidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::THRESHOLD);
	}

	template < WorldDimensionality dim >
	std::vector< double > mlp_ang_vel_agent< dim >::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		get_components(st.ship.ang_velocity, it);
		assert(it == nni.end());
		return nni;
	}

	template < WorldDimensionality dim >
	mlp_vel_and_angle_agent< dim >::mlp_vel_and_angle_agent(size_t num_layers, size_t num_outputs): mlp_agent< dim >(num_layers, num_outputs)
	{
		size_t const NumNNHidden = (NumNNInputs + num_nn_outputs) / 2;

		std::vector< unsigned int > layer_neurons(num_layers, (unsigned int)NumNNHidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::THRESHOLD);
	}

	template < WorldDimensionality dim >
	std::vector< double > mlp_vel_and_angle_agent< dim >::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		get_components(st.ship.lin_velocity, it);
		get_components(st.ship.ang_velocity, it);
		get_components(st.ship.orientation, it);
		assert(it == nni.end());
		return nni;
	}

	template < WorldDimensionality dim >
	mlp_vel_angle_pos_agent< dim >::mlp_vel_angle_pos_agent(size_t num_layers, size_t num_outputs): mlp_agent< dim >(num_layers, num_outputs)
	{
		size_t const NumNNHidden = (NumNNInputs + num_nn_outputs) / 2;

		std::vector< unsigned int > layer_neurons(num_layers, (unsigned int)NumNNHidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::THRESHOLD);
	}

	template < WorldDimensionality dim >
	std::vector< double > mlp_vel_angle_pos_agent< dim >::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		get_components(st.ship.lin_velocity, it);
		get_components(st.ship.ang_velocity, it);
		get_components(st.ship.position, it);
		get_components(st.ship.orientation, it);
		assert(it == nni.end());
		return nni;
	}


	template < WorldDimensionality dim >
	mlp_agent_factory< dim >::mlp_agent_factory(typename mlp_agent< dim >::Type type, size_t num_layers, size_t num_outputs):
		m_num_nn_layers(num_layers),
		m_num_nn_outputs(num_outputs)
	{
		m_type = type;
	}

	template < WorldDimensionality dim >
	i_agent* mlp_agent_factory< dim >::create()
	{
		switch(m_type)
		{
			case mlp_agent< dim >::AngVel:
			return new mlp_ang_vel_agent< dim >(m_num_nn_layers, m_num_nn_outputs);
			case mlp_agent< dim >::VelAngle:
			return new mlp_vel_and_angle_agent< dim >(m_num_nn_layers, m_num_nn_outputs);

		default:
			assert(false);
			return nullptr;
		}
	}


	template class passive_agent< WorldDimensionality::dim2D >;
	//template class passive_agent< WorldDimensionality::dim3D >;
	template class interactive_agent< WorldDimensionality::dim2D >;
	//template class interactive_agent< WorldDimensionality::dim3D >;
	template class mlp_agent< WorldDimensionality::dim2D >;
	//template class mlp_agent< WorldDimensionality::dim3D >;
	template class mlp_ang_vel_agent< WorldDimensionality::dim2D >;
	//template class mlp_ang_vel_agent< WorldDimensionality::dim3D >;
	template class mlp_vel_and_angle_agent< WorldDimensionality::dim2D >;
	//template class mlp_vel_and_angle_agent< WorldDimensionality::dim3D >;
	template class mlp_agent_factory< WorldDimensionality::dim2D >;
	//template class mlp_agent_factory< WorldDimensionality::dim3D >;
}


