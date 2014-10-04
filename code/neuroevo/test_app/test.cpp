
#include "systems/elevator/elevator_system_defn.h"
#include "systems/elevator/dumb_elevator_controller.h"

#include "params/param.h"
#include "params/param_accessor.h"
#include "params/schema_builder.h"
#include "params/cmdline.h"
/*
#include "wt_param_widgets/par_wgt.h"
#include "wt_param_widgets/container_par_wgt.h"
*/
#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WTextArea>
#include <Wt/WContainerWidget>
#include <Wt/WTime>
#include <Wt/WComboBox>
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

#include <boost/numeric/interval.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <random>

using namespace Wt;
using namespace std;



class TestApp: public WApplication
{
public:
	TestApp(const WEnvironment& env): WApplication(env)
	{
		setCssTheme("polished");
		styleSheet().addRule(".wb_numerical_input", "text-align: right;");

/*		useStyleSheet("nn_viz.css");

		auto vlayout = new Wt::WVBoxLayout();
		root()->setLayout(vlayout);
		vlayout->setContentsMargins(0, 0, 0, 0);

		auto load_box = new Wt::WComboBox;
		populate_database_box(load_box);
		vlayout->addWidget(load_box, 0, Wt::AlignTop | Wt::AlignLeft);

		auto load_btn = new Wt::WPushButton("Load");
		vlayout->addWidget(load_btn, 0, Wt::AlignTop | Wt::AlignLeft);

		auto vis = new WNNVisualiser;
		vlayout->addWidget(vis, 1);

		load_btn->clicked().connect(std::bind([=]
		{
			auto name = load_box->currentText().toUTF8();
			auto net = load_from_database(name);
			run_random_input(net.get());
			vis->set_neural_net(net);
		}));

		auto net = generate_random_mlp();
		run_random_input(net.get());
		vis->set_neural_net(net);
*/
		enableUpdates(true);
	}
};



WApplication* createApplication(const WEnvironment& env)
{
	return new TestApp(env);
}

#if 0

#include "neuralnet/interface/activation_functions_concept.h"

enum class ActivationFunctionActions {
	GetName,
	Evaluate,
};

namespace nnet {
	template<>
	struct enum_action_defn < ActivationFunctionActions, ActivationFunctionActions::GetName >
	{
		typedef std::string return_type;
	};

	template<>
	struct enum_action_defn < ActivationFunctionActions, ActivationFunctionActions::Evaluate >
	{
		typedef double return_type;
	};
}


struct Linear
{
	template <
		ActivationFunctionActions action,
		typename... Args
	>
	static inline auto do_action(Args...) -> typename nnet::enum_action_defn< ActivationFunctionActions, action >::return_type;

	template <>
	static inline std::string do_action< ActivationFunctionActions::GetName >()
	{
		return "Linear";
	}

	template <>
	static inline double do_action< ActivationFunctionActions::Evaluate, double >(double x)
	{
		return x;
	}
};

struct Sine
{
	template <
		ActivationFunctionActions action,
		typename... Args
	>
	static inline auto do_action(Args...) -> typename nnet::enum_action_defn< ActivationFunctionActions, action >::return_type;

	template <>
	static inline std::string do_action< ActivationFunctionActions::GetName >()
	{
		return "Sine";
	}

	template <>
	static inline double do_action< ActivationFunctionActions::Evaluate, double >(double x)
	{
		return std::sin(x);
	}
};

#endif


namespace sb = prm::schema;


int main(int argc, char* argv[])
{

	sb::schema_provider_map_handle sch_mp = std::make_shared< sb::schema_provider_map >();
	prm::qualified_path path;

	path += std::string{ "root" };
	path += std::string{ "container" };

	(*sch_mp)[path + prm::qualified_path{ "some_bool" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::boolean("some_bool", true);
		return s;
	};
	(*sch_mp)[path + prm::qualified_path{ "some_vec" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::vector2("some_vec", prm::vec2{ -1.0, 1.0 });
		return s;
	};
	(*sch_mp)[path + prm::qualified_path{ "some_random" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::random("some_random", 0.0, boost::none, boost::none);
		return s;
	};
	(*sch_mp)[path + prm::qualified_path{ "some_enum" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::enum_selection(
			"some_enum",
			std::vector < std::string > { "apple", "orange", "banana" },
			0,
			1
			);
		return s;
	};

	(*sch_mp)[path] = [=](prm::param_accessor& acc)
	{
		auto s = sb::list("container");
		sb::append(s, sch_mp->at(path + std::string{ "some_bool" })(acc));
		sb::append(s, sch_mp->at(path + std::string{ "some_vec" })(acc));
		sb::append(s, sch_mp->at(path + std::string{ "some_random" })(acc));
		sb::append(s, sch_mp->at(path + std::string{ "some_enum" })(acc));
		return s;
	};

	path.pop();

	(*sch_mp)[path + prm::qualified_path{ "some_int" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::integer("some_int", 0);
		return s;
	};
	(*sch_mp)[path + prm::qualified_path{ "some_string" }] = [](prm::param_accessor& acc)
	{
		auto s = sb::string("some_string", "default");
		return s;
	};

	(*sch_mp)[path] = [=](prm::param_accessor& acc)
	{
		auto s = sb::list("root");
		sb::append(s, sch_mp->at(path + std::string{ "container" })(acc));
		sb::append(s, sch_mp->at(path + std::string{ "some_int" })(acc));
		sb::append(s, sch_mp->at(path +std::string{ "some_string" })(acc));
		return s;
	};

	auto root = path;

#if 0
	auto defn = std::make_shared< sys::elev::elevator_system_defn >();
	defn->add_controller_defn("Preset", std::make_unique< sys::elev::dumb_elevator_controller_defn >());

	auto sch_mp = std::make_shared< sb::schema_provider_map >();
	auto root = defn->update_schema_providor(sch_mp, prm::qualified_path{});
#endif

	auto dummy_acc = prm::param_accessor{};
	auto sch = (*sch_mp)[root](dummy_acc);
	auto pt = prm::param_tree::generate_from_schema(sch);

	auto as_yaml = pt.convert_to_yaml();
	std::cout << YAML::Dump(as_yaml) << std::endl;

	pt = prm::param_tree::generate_from_yaml(as_yaml);

	prm::cmdline_processor cmdln{ sch_mp, pt };
	cmdln.enter_cmd_loop(std::cin, std::cout);


#if 0
	///////////////
	karma::rule< karma::ostream_iterator< char >, double() > val_rule =
		karma::double_;

	karma::rule< karma::ostream_iterator< char >, std::pair< double, double >() > range_rule =
		karma::double_ << karma::double_;
	/*
	karma::_1 = phx::construct< boost::fusion::tuple< double, double > >(
	phx::bind(&std::pair< double, double >::first, karma::_val),
	phx::bind(&std::pair< double, double >::second, karma::_val)
*/
	karma::rule< karma::ostream_iterator< char >, prm::random() > test_rule =
		(range_rule
		| val_rule)
		[
			karma::_1 = phx::bind(&prm::random::range, karma::_val)
		]

//		(val_rule | range_rule)
/*		[
			karma::_1 = phx::construct< boost::variant< double, boost::fusion::tuple< double, double > > >(
			phx::construct< boost::fusion::tuple< double, double > >(
			phx::val(5.5),
			phx::val(-1.1)
			)
			)
		]
		*/
		;

	test_rule.name("test_rule");
	val_rule.name("val_rule");
	range_rule.name("range_rule");

	karma::debug(test_rule);
	karma::debug(val_rule);
	karma::debug(range_rule);

	auto val = prm::random{
		std::make_pair(5.5, -1.1)
//		boost::fusion::make_tuple(5.5, -1.1)
	};

	std::cout << karma::format(
		test_rule,
		val

		//		range_rule,
		//		std::make_pair(5.5, -1.1)
		)
		<< std::endl;
	///////////////////
#endif


#if 0
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
#endif

	return WRun(argc, argv, &createApplication);
}

