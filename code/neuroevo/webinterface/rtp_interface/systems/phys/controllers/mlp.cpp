// mlp.cpp

#include "mlp.h"
#include "../bodies/test_body.h"
#include "../bodies/test_quadruped_body.h"
#include "../bodies/test_biped_body.h"
#include "../../../params/paramlist_par.h"
#include "../../../params/integer_par.h"
#include "../../../rtp_fixednn_genome_mapping.h"

#include <Wt/WComboBox>

#include <Box2D/Box2D.h>


namespace rtp_phys
{
	std::string const mlp_controller::Names[] = {
		"Test Controller",
		"Test Quadruped",
		"Test Biped",
	};

	
	mlp_controller::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	size_t mlp_controller::param_type::provide_num_child_params(rtp_param_manager* mgr) const
	{
		return 2;
	}

	rtp_named_param mlp_controller::param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
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

	std::pair< i_genome_mapping*, i_agent_factory* > mlp_controller::create_instance_evolvable(rtp_param param)
	{
		auto param_list = boost::any_cast< rtp_param_list >(param);

		Type type = boost::any_cast<Type>(param_list[0]);
		int num_layers = boost::any_cast<int>(param_list[1]);

		switch(type)
		{
			case TestType:
			{
				size_t const NumNNOutputs = 1;// TODO:  cfg.num_thrusters();
				size_t const NumPerHidden = (mlp_test_controller::NumNNInputs + NumNNOutputs + 1) / 2;
				return std::pair< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
						num_layers,
						mlp_test_controller::NumNNInputs,
						NumNNOutputs,
						NumPerHidden),
					new mlp_controller_factory(type, num_layers, NumPerHidden, NumNNOutputs));
			}

			case TestQuadruped:
			{
				size_t const NumNNOutputs = 8;
				size_t const NumPerHidden = (mlp_test_quadruped_controller::NumNNInputs + NumNNOutputs + 1) / 2;
				return std::pair< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
					num_layers,
					mlp_test_quadruped_controller::NumNNInputs,
					NumNNOutputs,
					NumPerHidden),
					new mlp_controller_factory(type, num_layers, NumPerHidden, NumNNOutputs));
			}

			case TestBiped:
			{
				size_t const NumNNOutputs = 5;
				size_t const NumPerHidden = (mlp_test_biped_controller::NumNNInputs + NumNNOutputs + 1) / 2;
				return std::pair< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
					num_layers,
					mlp_test_biped_controller::NumNNInputs,
					NumNNOutputs,
					NumPerHidden),
					new mlp_controller_factory(type, num_layers, NumPerHidden, NumNNOutputs));
			}

		default:
			assert(false);
			return std::pair< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

	mlp_controller::mlp_controller(size_t num_layers, size_t per_hidden, size_t num_outputs): num_nn_layers(num_layers), num_per_hidden(per_hidden), num_nn_outputs(num_outputs)
	{
		assert(num_nn_layers >= 2);
		assert(num_per_hidden >= 1);
		assert(num_nn_outputs >= 1);
	}

	mlp_test_controller::mlp_test_controller(size_t num_layers, size_t per_hidden, size_t num_outputs): mlp_controller(num_layers, per_hidden, num_outputs)
	{
		std::vector< unsigned int > layer_neurons(num_nn_layers, num_per_hidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
	}

	std::vector< double > mlp_test_controller::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		*it++ = ((test_body*)st.body.get())->joint->GetJointAngle();
		*it++ = ((test_body*)st.body.get())->joint->GetJointSpeed();
		assert(it == nni.end());
		return nni;
	}

	void mlp_test_controller::update(phys_system::state& st, phys_system::scenario_data sdata)
	{
		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);

		float torque = nn_outputs[0] * 200.0f;
		test_body* tbody = (test_body*)st.body.get();
		tbody->fore->ApplyTorque(torque, true);
		tbody->rear->ApplyTorque(-torque, true);
	}


	mlp_test_quadruped_controller::mlp_test_quadruped_controller(size_t num_layers, size_t per_hidden, size_t num_outputs): mlp_controller(num_layers, per_hidden, num_outputs)
	{
		std::vector< unsigned int > layer_neurons(num_nn_layers, num_per_hidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
	}

	std::vector< double > mlp_test_quadruped_controller::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		test_quadruped_body* qr = (test_quadruped_body*)st.body.get();
		*it++ = qr->rear_hip1->GetJointAngle();
		*it++ = qr->rear_hip1->GetJointSpeed();
		*it++ = qr->rear_hip2->GetJointAngle();
		*it++ = qr->rear_hip2->GetJointSpeed();
		*it++ = qr->rear_knee1->GetJointAngle();
		*it++ = qr->rear_knee1->GetJointSpeed();
		*it++ = qr->rear_knee2->GetJointAngle();
		*it++ = qr->rear_knee2->GetJointSpeed();
		*it++ = qr->fore_hip1->GetJointAngle();
		*it++ = qr->fore_hip1->GetJointSpeed();
		*it++ = qr->fore_hip2->GetJointAngle();
		*it++ = qr->fore_hip2->GetJointSpeed();
		*it++ = qr->fore_knee1->GetJointAngle();
		*it++ = qr->fore_knee1->GetJointSpeed();
		*it++ = qr->fore_knee2->GetJointAngle();
		*it++ = qr->fore_knee2->GetJointSpeed();
		*it++ = qr->torso->GetPosition().x;
		*it++ = qr->torso->GetPosition().y;
		*it++ = qr->torso->GetAngle();
		assert(it == nni.end());
		return nni;
	}

	void mlp_test_quadruped_controller::update(phys_system::state& st, phys_system::scenario_data sdata)
	{
		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);

		test_quadruped_body* qr = (test_quadruped_body*)st.body.get();
		float const MaxHipTorque = 100.0f;
		float const MaxKneeTorque = 60.0f;
		qr->torso->ApplyTorque(-nn_outputs[0] * MaxHipTorque, true);
		qr->rear_upper1->ApplyTorque(nn_outputs[0] * MaxHipTorque, true);
		qr->torso->ApplyTorque(-nn_outputs[1] * MaxHipTorque, true);
		qr->rear_upper2->ApplyTorque(nn_outputs[1] * MaxHipTorque, true);
		qr->rear_upper1->ApplyTorque(-nn_outputs[2] * MaxKneeTorque, true);
		qr->rear_lower1->ApplyTorque(nn_outputs[2] * MaxKneeTorque, true);
		qr->rear_upper2->ApplyTorque(-nn_outputs[3] * MaxKneeTorque, true);
		qr->rear_lower2->ApplyTorque(nn_outputs[3] * MaxKneeTorque, true);
		qr->torso->ApplyTorque(-nn_outputs[4] * MaxHipTorque, true);
		qr->fore_upper1->ApplyTorque(nn_outputs[4] * MaxHipTorque, true);
		qr->torso->ApplyTorque(-nn_outputs[5] * MaxHipTorque, true);
		qr->fore_upper2->ApplyTorque(nn_outputs[5] * MaxHipTorque, true);
		qr->fore_upper1->ApplyTorque(-nn_outputs[6] * MaxKneeTorque, true);
		qr->fore_lower1->ApplyTorque(nn_outputs[6] * MaxKneeTorque, true);
		qr->fore_upper2->ApplyTorque(-nn_outputs[7] * MaxKneeTorque, true);
		qr->fore_lower2->ApplyTorque(nn_outputs[7] * MaxKneeTorque, true);
	}


	mlp_test_biped_controller::mlp_test_biped_controller(size_t num_layers, size_t per_hidden, size_t num_outputs): mlp_controller(num_layers, per_hidden, num_outputs)
	{
		std::vector< unsigned int > layer_neurons(num_nn_layers, num_per_hidden);
		layer_neurons[0] = NumNNInputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
	}

	std::vector< double > mlp_test_biped_controller::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		test_biped_body* bp = (test_biped_body*)st.body.get();
		*it++ = bp->pelvis_joint->GetJointAngle();
		*it++ = bp->pelvis_joint->GetJointSpeed();
		*it++ = bp->hip1->GetJointAngle();
		*it++ = bp->hip1->GetJointSpeed();
		*it++ = bp->hip2->GetJointAngle();
		*it++ = bp->hip2->GetJointSpeed();
		*it++ = bp->knee1->GetJointAngle();
		*it++ = bp->knee1->GetJointSpeed();
		*it++ = bp->knee2->GetJointAngle();
		*it++ = bp->knee2->GetJointSpeed();
		assert(it == nni.end());
		return nni;
	}

	void mlp_test_biped_controller::update(phys_system::state& st, phys_system::scenario_data sdata)
	{
		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);

		test_biped_body* bp = (test_biped_body*)st.body.get();
		float const MaxPelvicTorque = 75.0f;
		float const MaxHipTorque = 75.0f;
		float const MaxKneeTorque = 75.0f;
		bp->torso->ApplyTorque(-nn_outputs[0] * MaxPelvicTorque, true);
		bp->pelvis->ApplyTorque(nn_outputs[0] * MaxPelvicTorque, true);
		bp->pelvis->ApplyTorque(-nn_outputs[1] * MaxHipTorque, true);
		bp->upper1->ApplyTorque(nn_outputs[1] * MaxHipTorque, true);
		bp->pelvis->ApplyTorque(-nn_outputs[2] * MaxHipTorque, true);
		bp->upper2->ApplyTorque(nn_outputs[2] * MaxHipTorque, true);
		bp->upper1->ApplyTorque(-nn_outputs[3] * MaxKneeTorque, true);
		bp->lower1->ApplyTorque(nn_outputs[3] * MaxKneeTorque, true);
		bp->upper2->ApplyTorque(-nn_outputs[4] * MaxKneeTorque, true);
		bp->lower2->ApplyTorque(nn_outputs[4] * MaxKneeTorque, true);
	}

	
	mlp_controller_factory::mlp_controller_factory(mlp_controller::Type type, size_t num_layers, size_t num_per_hidden, size_t num_outputs):
		m_num_nn_layers(num_layers),
		m_num_per_hidden(num_per_hidden),
		m_num_nn_outputs(num_outputs)
	{
		m_type = type;
	}

	i_agent* mlp_controller_factory::create()
	{
		switch(m_type)
		{
			case mlp_controller::TestType:
			return new mlp_test_controller(m_num_nn_layers, m_num_per_hidden, m_num_nn_outputs);

			case mlp_controller::TestQuadruped:
			return new mlp_test_quadruped_controller(m_num_nn_layers, m_num_per_hidden, m_num_nn_outputs);

			case mlp_controller::TestBiped:
			return new mlp_test_biped_controller(m_num_nn_layers, m_num_per_hidden, m_num_nn_outputs);

			default:
			assert(false);
			return nullptr;
		}
	}
}


