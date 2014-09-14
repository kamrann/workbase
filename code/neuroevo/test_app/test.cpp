
#include "yaml_test.h"

#include "neuralnet/implementations/mlf/mlp.h"
#include "neuralnet/interface/input.h"
#include "neuralnet/interface/output.h"
#include "neuralnet/interface/neuron_access.h"
#include <doublefann.h>
#include <fann_cpp.h>


//
#include "../webinterface/rtp_interface/systems/rtp_system.h"
//

#include "params/param.h"

#include "wt_param_widgets/par_wgt.h"
#include "wt_param_widgets/container_par_wgt.h"
#include "wt_param_widgets/param_accessor.h"

#include "neuralnet/implementations/mlf/mlp.h"
#include "neuralnet/visualisation/wt_visualiser.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WTextArea>
#include <Wt/WContainerWidget>
#include <Wt/WTime>
#include <Wt/WSelectionBox>
#include <Wt/WGroupBox>
#include <Wt/WText>
#include <Wt/WPanel>
#include <Wt/WTabWidget>
#include <Wt/WCheckBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>

#include <yaml-cpp/yaml.h>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <random>

using namespace Wt;
using namespace std;
namespace dbo = Wt::Dbo;

using nnet::vis::wt::WNNVisualiser;


class TestApp: public WApplication
{
public:
	TestApp(const WEnvironment& env): WApplication(env)
	{
		setCssTheme("polished");
		styleSheet().addRule(".wb_numerical_input", "text-align: right;");

		useStyleSheet("nn_viz.css");

		root()->setLayout(new Wt::WHBoxLayout());

		nnet::mlp::layer_counts_t lcounts{ 2, 1, 1 };

		auto mlp = std::make_shared< nnet::mlp >();
		mlp->create(lcounts);
		mlp->set_activation_function(1, nnet::ActivationFn::Linear);
		mlp->set_activation_function(2, nnet::ActivationFn::LinearBounded);

		std::default_random_engine rgen;
		rgen.seed(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		std::uniform_real_distribution<> dist(-1.0, 1.0);
		std::vector< double > weights(mlp->num_connections(), 0.0);
		for(auto& w : weights)
		{
			w = dist(rgen);
		}

		nnet::input input(lcounts.front());
		for(size_t i = 0; i < input.size(); ++i)
		{
			input[i] = dist(rgen);
		}

		mlp->load_weights(weights);
		auto mlp_out = mlp->execute(input);

		auto vis = new WNNVisualiser;
		vis->set_neural_net(mlp);

		root()->layout()->addWidget(vis);

		enableUpdates(true);
	}
};



WApplication* createApplication(const WEnvironment& env)
{
	return new TestApp(env);
}

#include "neuralnet/interface/activation_functions_concept.h"

int main(int argc, char* argv[])
{
/*	auto fn1 = nnet::Fn::FnA;
	auto fn2 = nnet::Fn::FnB;
	auto result1 = nnet::ActivationFunc<>::dispatch< nnet::Actions::Apply >(fn1, 1.0);
	auto result2 = nnet::ActivationFunc<>::dispatch< nnet::Actions::Apply >(fn2, 1.0);
*/

	auto lcounts = nnet::mlp::layer_counts_t{ 6, 4, 2 };

	nnet::mlp mlp;
	mlp.create(lcounts);

	FANN::neural_net fann;
	fann.create_standard_array(lcounts.size(), &lcounts[0]);
	fann.set_activation_function_hidden(FANN::activation_function_enum::LINEAR);
	fann.set_activation_function_output(FANN::activation_function_enum::LINEAR);
	fann.set_activation_steepness_hidden(1.0);
	fann.set_activation_steepness_output(1.0);

	std::cout << "Neurons: " << mlp.total_neuron_count() << " / " << fann.get_total_neurons() << std::endl;
	std::cout << "Connections: " << mlp.num_connections() << " / " << fann.get_total_connections() << std::endl;

	std::default_random_engine rgen;
	rgen.seed(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
	std::uniform_real_distribution<> dist(-1.0, 1.0);
	std::vector< double > weights(mlp.num_connections(), 0.0);
	for(auto& w : weights)
	{
		w = dist(rgen);
	}

	nnet::input input(lcounts.front());
	for(size_t i = 0; i < input.size(); ++i)
	{
		input[i] = dist(rgen);
	}

	mlp.load_weights(weights);
	auto mlp_out = mlp.execute(input);

	std::vector< fann_connection > conns(fann.get_total_connections());
	fann.get_connection_array(&conns[0]);
	for(size_t i = 0; i < fann.get_total_connections(); ++i)
	{
		conns[i].weight = weights[i];
	}
	fann.set_weight_array(&conns[0], conns.size());
	auto fann_out = fann.run(&input[0]);

	std::cout << "Output: ";
	std::cout << "[ ";
	for(auto val : mlp_out)
	{
		std::cout << val << " ";
	}
	std::cout << "]";

	std::cout << " / ";

	std::cout << "[ ";
	for(size_t i = 0; i < lcounts.back(); ++i)
	{
		std::cout << fann_out[i] << " ";
	}
	std::cout << "]";

	std::cout << std::endl;

	return WRun(argc, argv, &createApplication);
}

