
#include "systems/elevator/elevator_system_defn.h"
#include "systems/elevator/elevator_agent_defn.h"
#include "systems/elevator/dumb_elevator_controller.h"

#include "system_sim/system.h"

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

#include <boost/variant/static_visitor.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;


struct vis: public boost::static_visitor < void >
{
	template < typename T >
	result_type operator() (T const&)
	{
		std::cout << T::value << std::endl;
	}
};


#include "genetic_algorithm/genalg.h"
#include "genetic_algorithm/genetic_population.h"
#include "genetic_algorithm/fixed_binary_genome.h"


int main(int argc, char* argv[])
{
	ga::rgen_t rgen;
	rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
	ga::genalg alg{ rgen };

	typedef ga::fixed_binary_genome genome_t;

	size_t const pop_size = 200;
	size_t const num_generations = 1000;

	size_t const bits_per_character = 8;
	std::string target_str = "cameron";
	auto const string_length = target_str.length();
	size_t const gn_length = string_length * bits_per_character;

	ga::basic_array_crossover< genome_t > _crossover{ gn_length };
	alg.crossover_fn() = [&_crossover](ga::genome const& p1, ga::genome const& p2, ga::rgen_t& rgen)
	{
		return std::make_unique< genome_t >(_crossover(p1.as< genome_t >(), p2.as< genome_t >(), rgen));
	};
	ga::basic_binary_mutation _mutation{};
	alg.mutation_fn() = [&_mutation](ga::genome& gn, double const rate, ga::rgen_t& rgen)
	{
		_mutation(gn.as< genome_t >(), rate, rgen);
	};

	alg.crossover_rate_fn() = []()
	{
		return 1.0;
	};

	alg.mutation_rate_fn() = []()
	{
		return 0.05;
	};

	auto decode_fn = [=](genome_t const& gn) -> std::string
	{
		std::string result;
		auto it = gn.cbegin();//std::begin(gn);
		for(size_t i = 0; i < string_length; ++i)
		{
			char ch = 0;
			for(size_t b = 0; b < bits_per_character; ++b)
			{
				if(*it != 0)
				{
					ch |= (1 << b);
				}
				++it;
			}

			result += ch;
		}
		return result;
	};

	auto fitness_fn = [=](std::string str) -> double
	{
		size_t num_correct = 0;
		for(size_t i = 0; i < string_length; ++i)
		{
			if(str[i] == target_str[i])
			{
				++num_correct;
			}
		}
		return (double)(num_correct) / string_length;
	};

	ga::genetic_population pop;
	pop.resize(pop_size);
	
	// Initialize first generation
	for(auto& idv : pop)
	{
		idv.gn = ga::genome{ std::make_unique< genome_t >(genome_t::generate_random(gn_length, rgen)) };
	}

	for(size_t g = 0; g < num_generations; ++g)
	{
		// Evaluate solutions
		std::string best;
		double highest_fitness = -1.0;

		for(auto& idv : pop)
		{
			auto str = decode_fn(idv.gn.as< genome_t >());
			idv.fitness = fitness_fn(str);

			if(idv.fitness > highest_fitness)
			{
				best = str;
				highest_fitness = idv.fitness;
			}
		}

		std::cout << "Generation #" << g << ": [" << best << "] (" << highest_fitness << ")" << std::endl;

		// Create next generation
		auto next = alg.next_generation(pop);
		pop = std::move(next);
	}

	// Evaluate final solutions
	std::string best;
	double highest_fitness = -1.0;
	for(auto idv : pop)
	{
		auto str = decode_fn(idv.gn.as< genome_t >());
		idv.fitness = fitness_fn(str);

		if(idv.fitness > highest_fitness)
		{
			best = str;
			highest_fitness = idv.fitness;
		}
	}

	std::cout << "Best solution: " << best << std::endl;


#if 0
	typedef std::string::const_iterator iter_t;

	struct one_t{ enum { value = 1 }; };
	struct two_t{ enum { value = 2 }; };

	qi::rule< iter_t, boost::variant< one_t, two_t >() > p;

	{
		qi::rule< iter_t, one_t() > one = qi::lit("one")[qi::_val = phx::val(one_t{})];
		qi::rule< iter_t, two_t() > two = qi::lit("two")[qi::_val = phx::val(two_t{})];

		p = one.copy() | two.copy();
	}

	while(true)
	{
		boost::variant< one_t, two_t > attr;

		std::string input;
		std::getline(std::cin, input);
		auto it = std::begin(input);
		auto result = qi::phrase_parse(it, std::end(input), p, qi::space_type{}, attr);
		if(!result)
		{
			std::cout << "parse failure" << std::endl;
		}
		else
		{
			vis v{};
			boost::apply_visitor(v, attr);
		}
	}
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

