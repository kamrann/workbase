// mlp.cpp

#include "mlp.h"
#include "../bodies/test_body.h"
#include "../bodies/test_quadruped_body.h"
#include "../bodies/test_biped_body.h"
#include "../bodies/basic_spaceship.h"
#include "../../../params/paramlist_par.h"
#include "../../../params/integer_par.h"
#include "../../../rtp_fixednn_genome_mapping.h"

#include "wt_param_widgets/pw_yaml.h"

#include <Wt/WComboBox>

#include <Box2D/Box2D.h>


namespace rtp_phys
{
	std::string const mlp_controller::Names[] = {
		"Test Controller",
		"Test Quadruped",
		"Test Biped",
		"Spaceship",
	};
}

namespace YAML {
	template <>
	struct convert< rtp_phys::mlp_controller::Type >
	{
		static Node encode(rtp_phys::mlp_controller::Type const& rhs)
		{
			return Node(rtp_phys::mlp_controller::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_phys::mlp_controller::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp_phys::mlp_controller::Type > const mapping_;
	};

	std::map< std::string, rtp_phys::mlp_controller::Type > const convert< rtp_phys::mlp_controller::Type >::mapping_ = {
		{ "Test Controller", rtp_phys::mlp_controller::Type::TestType },
		{ "Test Quadruped", rtp_phys::mlp_controller::Type::TestQuadruped },
		{ "Test Biped", rtp_phys::mlp_controller::Type::TestBiped },
		{ "Spaceship", rtp_phys::mlp_controller::Type::Spaceship },
	};
}

namespace rtp_phys {

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

	namespace sb = prm::schema;

	YAML::Node mlp_controller::get_schema(YAML::Node const& param_vals)
	{
		auto schema = sb::list("mlp_controller");

		auto type = sb::enum_selection("Inputs", { begin(Names), end(Names) });
		sb::on_update(type);
		sb::append(schema, type);

		sb::append(schema, sb::integer("Num Layers", 3, 2, 5));

		return schema;
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

			case Spaceship:
			{
				size_t const NumNNOutputs = 4;	// TODO:
				size_t const NumPerHidden = (mlp_all_inputs_spaceship_controller::NumNNInputs + NumNNOutputs + 1) / 2;
				return std::pair< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
					num_layers,
					mlp_all_inputs_spaceship_controller::NumNNInputs,
					NumNNOutputs,
					NumPerHidden),
					new mlp_controller_factory(type, num_layers, NumPerHidden, NumNNOutputs));
			}

		default:
			assert(false);
			return std::pair< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

	std::pair< i_genome_mapping*, i_agent_factory* > mlp_controller::create_instance_evolvable(YAML::Node const& param)
	{
		auto type = prm::find_value(param, "Inputs").as< Type >();
		int num_layers = prm::find_value(param, "Num Layers").as< int >();

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

			case Spaceship:
			{
				size_t const NumNNOutputs = 4;	// TODO:
				size_t const NumPerHidden = (mlp_all_inputs_spaceship_controller::NumNNInputs + NumNNOutputs + 1) / 2;
				return std::pair< i_genome_mapping*, i_agent_factory* >(
					new fixednn_genome_mapping(
					num_layers,
					mlp_all_inputs_spaceship_controller::NumNNInputs,
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


	mlp_spaceship_controller::mlp_spaceship_controller(size_t num_layers, size_t per_hidden, size_t num_outputs): mlp_controller(num_layers, per_hidden, num_outputs)
	{
		
	}

	void mlp_spaceship_controller::create_nn(size_t num_inputs)
	{
		std::vector< unsigned int > layer_neurons(num_nn_layers, num_per_hidden);
		layer_neurons[0] = num_inputs;
		layer_neurons[num_nn_layers - 1] = num_nn_outputs;
		nn.create_standard_array(num_nn_layers, &layer_neurons[0]);

		nn.set_activation_steepness_hidden(1.0);
		nn.set_activation_steepness_output(1.0);

		nn.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
		nn.set_activation_function_output(FANN::THRESHOLD);
	}

	void mlp_spaceship_controller::update(phys_system::state& st, phys_system::scenario_data sdata)
	{
		std::vector< double > nn_inputs = map_nn_inputs(st);
		double* nn_outputs = nn.run((double*)&nn_inputs[0]);
		thruster_base::thruster_activation ta(num_nn_outputs, false);
		std::transform(nn_outputs, nn_outputs + num_nn_outputs, ta.begin(), [](double d_act) {
			return d_act > 0.0;
		});

		basic_spaceship* ss = static_cast< basic_spaceship* >(st.body.get());
		float const ThrusterForce = 100.0f;	// TODO: Property of the ship/thrusters

		for(size_t i = 0; i < ss->m_t_cfg->num_thrusters(); ++i)
		{
			if(ta[i])
			{
				b2Vec2 pos = ss->m_half_width * b2Vec2((*ss->m_t_cfg)[i].pos[0], (*ss->m_t_cfg)[i].pos[1]);
				b2Vec2 dir((*ss->m_t_cfg)[i].dir[0], (*ss->m_t_cfg)[i].dir[1]);
				ss->apply_force_local(ThrusterForce * dir, pos);

				ss->m_t_system[i].t.engage();
			}
			else
			{
				ss->m_t_system[i].t.cool_down(1.0f / 60.0f);	// TODO: Hard copied from phys_system::update()
			}
		}
	}

	size_t const mlp_all_inputs_spaceship_controller::NumNNInputs = 6;

	mlp_all_inputs_spaceship_controller::mlp_all_inputs_spaceship_controller(size_t num_layers, size_t per_hidden, size_t num_outputs):
		mlp_spaceship_controller(num_layers, per_hidden, num_outputs)
	{
		create_nn(NumNNInputs);
	}

	std::vector< double > mlp_all_inputs_spaceship_controller::map_nn_inputs(state_t const& st)
	{
		std::vector< double > nni(NumNNInputs);
		std::vector< double >::iterator it = nni.begin();
		basic_spaceship* ss = static_cast< basic_spaceship* >(st.body.get());
		b2Vec2 const linvel = ss->get_linear_velocity();
		// TODO: Temp map linear velocity vector into local coordinates, so orientation is not needed
		//linvel = ss->m_body->GetLocalVector(linvel);
		*it++ = linvel.x;
		*it++ = linvel.y;
		*it++ = ss->get_angular_velocity();
		b2Vec2 const pos = ss->get_position();
		*it++ = pos.x;
		*it++ = pos.y;
		// TODO: Could it be better for NN to input orientation as 2 inputs, absolute val (0-Pi), plus a sign input (-1,1)?
		// Feel as though it may be hard for it to deal with an input that can flip at a boundary from +Pi to -Pi.
		*it++ = ss->get_orientation();
		assert(it == nni.end());
		return nni;
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

			case mlp_controller::Spaceship:
			return new mlp_all_inputs_spaceship_controller(m_num_nn_layers, m_num_per_hidden, m_num_nn_outputs);

			default:
			assert(false);
			return nullptr;
		}
	}
}


