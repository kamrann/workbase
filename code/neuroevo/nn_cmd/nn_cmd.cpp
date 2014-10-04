// nn_cmd.cpp : Defines the entry point for the console application.
//

#include "basic_types.h"
#include "Wt Displays/wt_server.h"

#include "evo_database/evo_db.h"
#include "wt_param_widgets/param_accessor.h"
#include "wt_param_widgets/pw_yaml.h"

#include "neuralnet/interface/neuralnet.h"
#include "neuralnet/interface/input.h"
#include "neuralnet/interface/output.h"
#include "neuralnet/interface/activation_functions.h"
#include "neuralnet/interface/neuron_access.h"
#include "neuralnet/interface/connection_access.h"
#include "neuralnet/interface/modifiable.h"
#include "neuralnet/interface/algorithm.h"
#include "neuralnet/interface/stream_io.h"

#include "neuralnet/visualisation/text_output.h"

#include "neuralnet/implementations/mlf/mlp.h"

#include "util/interval_io.h"

#include <Wt/Dbo/FixedSqlConnectionPool>

#include <boost/optional.hpp>

#include <string>
#include <iostream>
#include <map>
#include <cctype>
#include <algorithm>
#include <regex>
#include <list>
#include <set>
#include <stack>
#include <memory>
#include <random>
#include <chrono>


struct str_compare
{
	inline bool operator() (std::string s1, std::string s2) const
	{
		std::transform(s1.begin(), s1.end(), s1.begin(), std::toupper);
		std::transform(s2.begin(), s2.end(), s2.begin(), std::toupper);
		return s1 < s2;
	}
};


typedef std::set< char > char_set;
typedef std::pair< char, char > bracket_chars;
typedef std::set< bracket_chars > bracket_set;

// Tokenizes a string on a set of delimiters, but without breaking up substrings enclosed by brackets
// TODO: A more complete version would perhaps return a nested structure of tokenized strings within nested and/or
// different types of brackets
arg_list bracket_tokenize(
	std::string const& str,								// String to tokenize
	//	char_set const& delims = { ' ' },					// Standard delimiter set
	std::string const& re_delim = "\\s+",
	bracket_set const& brackets = { { '(', ')' } }		// Bracket definitions
	)
{
	char_set open_chars, close_chars;
	std::transform(std::begin(brackets), std::end(brackets), std::inserter(open_chars, std::end(open_chars)), [](bracket_chars const& bc)
	{
		return bc.first;
	});
	std::transform(std::begin(brackets), std::end(brackets), std::inserter(close_chars, std::end(close_chars)), [](bracket_chars const& bc)
	{
		return bc.second;
	});

	std::map< char, char > brk_map;
	std::copy(std::begin(brackets), std::end(brackets), std::inserter(brk_map, std::end(brk_map)));

	arg_list tokens;
	// Retain a stack of the brackets opened at any given point, identified by the 'close' character
	std::stack< char/*bracket_set::iterator*/ > opened;
	auto it = std::begin(str);
	auto const end = std::end(str);
	std::string::const_iterator open_pos;
	while(it != end)
	{
		auto next_opened = std::find_if(it, end, [&open_chars](char c){ return open_chars.find(c) != std::end(open_chars); });
		auto next_closed = std::find_if(it, end, [&close_chars](char c){ return close_chars.find(c) != std::end(close_chars); });

		if(next_closed != end && (next_closed < next_opened || 
			(next_closed == next_opened && !opened.empty() && opened.top() == *next_closed)))
		{
			// Next up is a closing bracket
			if(*next_closed != opened.top())
			{
				auto msg = std::string{ "Bracket mismatch at: " } +*next_closed + " - Expected: " + opened.top();
				throw std::runtime_error(msg);
			}

			// Valid closing bracket
			it = next_closed;
			++it;
			opened.pop();
			if(opened.empty())
			{
				// Outer brackets closed, add token
				tokens.emplace_back(std::string{ open_pos, it });
			}
		}
		else
		{
			// Next up is an opening bracket, or there are no more brackets

			if(opened.empty())
			{
				// Tokenize by delims up to next opened bracket
				std::regex rgx(re_delim);
				tokens.insert(
					tokens.end(),
					std::sregex_token_iterator(it, next_opened, rgx, -1),
					std::sregex_token_iterator()
					);

				// TODO: nicer way to deal with this
				auto temp = std::remove_if(std::begin(tokens), std::end(tokens), [](std::string const& tk)
				{
					return tk.empty();
				});
				tokens.erase(temp, std::end(tokens));
			}

			it = next_opened;
			if(next_opened != end)
			{
				// Found an opening bracket
				if(opened.empty())
				{
					open_pos = next_opened;
				}
				opened.push(brk_map.at(*next_opened));

				++it;
			}
		}
	}

	if(!opened.empty())
	{
		throw std::runtime_error("Unclosed brackets");
	}

	return tokens;
}


struct state_t
{
	std::unique_ptr< nnet::i_neuralnet > net;
	boost::optional< nnet::input > input;
	boost::optional< nnet::input_range > input_ranges;

	state_t(): net{}, input{ boost::none }, input_ranges{ boost::none }
	{}

	void assert_net() const
	{
		if(!net)
		{
			throw std::runtime_error{ "No network" };
		}
	}

	void assert_input() const
	{
		if(!input)
		{
			throw std::runtime_error{ "No input" };
		}
	}

	void assert_input_ranges() const
	{
		if(!input_ranges)
		{
			throw std::runtime_error{ "No input ranges specified" };
		}
	}
};

state_t state;
std::default_random_engine rgen;

std::unique_ptr< Wt::Dbo::SqlConnectionPool > db_cp;// = evodb_session::create_connection_pool("../webinterface/evo.db");
std::unique_ptr< evodb_session > db_session;// (*db_cp);


namespace cmd {

	enum class Command {
		exit,
		create,
		load,
		info,
		input,
		input_ranges,
		fn,
		weights,
		run,
		output_ranges,

		ping,
		server_test,
	};

	void create_mlp(arg_list args)
	{
		nnet::mlp::layer_counts_t layer_counts;
		std::transform(
			args.begin(),
			args.end(),
			std::back_inserter(layer_counts),
			[](std::string const& str)
		{
			size_t pos;
			auto val = std::stoul(str, &pos);
			if(pos != str.length() || val == 0)
			{
				throw std::runtime_error("Invalid layer count");
			}
			else
			{
				return val;
			}
		});

		if(layer_counts.size() < 2)
		{
			std::cout << "MLP requires minimum of 2 layers" << std::endl;
			return;
		}

		auto mlp = std::make_unique< nnet::mlp >();
		mlp->create(layer_counts);
		state.net = std::move(mlp);
		state.input = boost::none;
		state.input_ranges = boost::none;
	}

	void create(arg_list args)
	{
		if(args.empty())
		{
			std::cout << "usage: create <network-type> <network-params>" << std::endl;
			return;
		}

		auto type = args.front();
		args.pop_front();
		if(type == "mlp")	// todo: switch
		{
			create_mlp(std::move(args));
			return;
		}

		std::cout << "Unrecognized network type" << std::endl;
	}

	void load(arg_list args)
	{
		if(args.empty())
		{
			std::cout << "usage: load <network-filename>" << std::endl;
			return;
		}

		auto filename = args.front();
		if(filename[0] == '\"' && filename[filename.length() - 1] == '\"')
		{
			filename = filename.substr(1, filename.length() - 2);
		}
		// TODO: for now just database

		// Temporary hackage
		dbo::Transaction t(*db_session);
		auto query = db_session->find< named_genome >().where("name = ?").bind(filename);
		auto query_result = query.resultValue();

		auto pgn = query_result->gn;
		auto pevo_run = pgn->generation->run;
		auto run_params = YAML::Load(pevo_run->sys_params);
		//
		std::cout << pevo_run->sys_params << std::endl;
		//
		prm::param_accessor run_params_acc(run_params);
		run_params_acc.push_relative_path(std::string{ "sim_params" });
		run_params_acc.push_relative_path(std::string{ "evolved_container" });
		run_params_acc.push_relative_path(std::string{ "evolved_list" });
		auto path = run_params_acc.get_current_path();
		path.leaf().set_index(1);
		path += std::string{ "evolved" };
		run_params_acc = run_params_acc.from_absolute(path);

//		auto num_layers = run_params_acc["mlp_num_layers"].as< int >();
		auto num_inputs = run_params_acc["mlp_inputs"].size();

		nnet::mlp::layer_counts_t layer_counts;
		layer_counts.push_back(num_inputs);

		std::map< unsigned int, size_t > hidden_layer_counts;
		auto hidden_list_node = run_params_acc["hidden_layer_list"];
		for(auto const& node : hidden_list_node)
		{
			auto inst_num = node.first.as< unsigned int >();
			auto rel = prm::qualified_path{ "hidden_layer_list" };
			rel.leaf().set_index(inst_num);
			run_params_acc.push_relative_path(rel);

			hidden_layer_counts[inst_num] = run_params_acc["hl_neuron_count"].as< size_t >();

			run_params_acc.pop_relative_path();
		}

		for(auto const& hl : hidden_layer_counts)
		{
			layer_counts.push_back(hl.second);
		}

		auto num_outputs = 1;	// !!!!!!!!!!! TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!

		layer_counts.push_back(num_outputs);

		nnet::ActivationFnType hidden_fn = (nnet::ActivationFnType)std::stoi(
			run_params_acc["hidden_activation_fn"][0].as< std::string >());
		auto hidden_steepness = run_params_acc["hidden_steepness"].as< double >();
		nnet::ActivationFnType output_fn = (nnet::ActivationFnType)std::stoi(
			run_params_acc["output_activation_fn"][0].as< std::string >());
		auto output_steepness = run_params_acc["output_steepness"].as< double >();

		//size_t const NumPerHidden = (num_inputs + num_outputs + 1) / 2;

		// From the binary
		auto const& bytes = pgn->encoding;
		std::vector< double > weights;
		auto const bytes_per_val = sizeof(double);
		if(bytes.size() % bytes_per_val != 0)
		{
			std::cout << "Failed" << std::endl;
			return;
		}
		auto const num_vals = bytes.size() / bytes_per_val;
		size_t byte_index = 0;
		for(size_t v = 0; v < num_vals; ++v)
		{
			double val{ 0 };
			unsigned char* ptr = reinterpret_cast<unsigned char*>(&val);
			for(size_t i = 0; i < bytes_per_val; ++i, ++byte_index, ++ptr)
			{
				*ptr = bytes[byte_index];
			}
			weights.push_back(val);
		}
		//

		auto mlp = std::make_unique< nnet::mlp >();
		mlp->create(layer_counts);

		for(size_t l = 1; l < layer_counts.size() - 1; ++l)
		{
			mlp->set_activation_function_layer(l, nnet::activation_function{ hidden_fn, hidden_steepness });
		}
		mlp->set_activation_function_layer(layer_counts.size() - 1, nnet::activation_function{ output_fn, output_steepness });

		mlp->load_weights(weights);
		
		state.net = std::move(mlp);
		state.input = boost::none;
		state.input_ranges = boost::none;
	}

	void info(arg_list args)
	{
		state.assert_net();

		// TODO: virtual get network type
		//input({});
	}

	nnet::range_array_t parse_ranges(size_t count, arg_list& args)
	{
		nnet::range_array_t ranges;
		for(auto const& in : args)
		{
			if(in == "*")
			{
				if(ranges.size() == 1)
				{
					ranges.resize(count, ranges.front());
					break;
				}
				else
				{
					throw std::runtime_error{ "* must follow a single range" };
				}
			}

			std::istringstream strm{ in };
			nnet::range_t rg;
			strm >> rg;
			if(!strm)
			{
				throw std::runtime_error("Invalid range");
			}

/*			strm >> std::ws;
			if(ranges.empty() && strm.peek() == std::char_traits< char >::to_int_type('*'))
			{
				ranges.resize(state.net->input_count(), rg);
				break;
			}
			else
*/			{
				ranges.push_back(rg);
			}
		}

		if(ranges.size() != count)
		{
			auto msg = std::string{ "Incorrect number of ranges. " } +
				std::to_string(count) +
				" required";
			throw std::runtime_error(msg);
		}

		return ranges;
	}

	void input(arg_list args)
	{
		state.assert_net();

		if(args.empty())
		{
			//std::cout << "usage: input <input-value-list>" << std::endl;
			if(state.input)
			{
				std::cout << state.input->size() << " inputs: " << *state.input << std::endl;
			}
			else
			{
				std::cout << "No input" << std::endl;
			}
			return;
		}

		if(args.front() == "random")
		{
			args.pop_front();
			nnet::input_range_array_t ranges;
			if(args.empty())
			{
				if(state.input_ranges)
				{
					ranges = { state.input_ranges->begin(), state.input_ranges->end() };
				}
				else
				{
					throw std::runtime_error{ "no ranges specified" };
				}
			}
			else
			{
				ranges = parse_ranges(state.net->input_count(), args);
			}

			nnet::input_array_t inputs;
			for(auto const& rg : ranges)
			{
				// TODO: !!!!!!!!!!!!!!! implementation for interval and multi_interval, this is quick hack 
				std::uniform_real_distribution<> dist(rg.lower_bound(), rg.upper_bound());
				inputs.emplace_back(dist(rgen));
			}
			state.input = nnet::input{ inputs };
			return;
		}

		if(args.size() != state.net->input_count())
		{
			auto msg = std::string{ "Incorrect number of inputs. " } +
				std::to_string(state.net->input_count()) +
				" required";
			throw std::runtime_error(msg);
		}

		nnet::input_array_t inputs;
		for(auto const& in : args)
		{
			size_t pos;
			auto val = std::stod(in, &pos);
			if(pos != in.length())
			{
				throw std::runtime_error("Invalid input value");
			}

			inputs.push_back(val);
		}

		state.input = nnet::input{ inputs };
	}

	void input_ranges(arg_list args)
	{
		state.assert_net();

		if(args.empty())
		{
			if(state.input_ranges)
			{
				std::cout << state.input_ranges->size() << " inputs with ranges: " << *state.input_ranges << std::endl;
			}
			else
			{
				std::cout << "No input ranges specified" << std::endl;
			}
			return;
		}

		nnet::input_range_array_t ranges{ parse_ranges(state.net->input_count(), args) };
		state.input_ranges = nnet::input_range{ ranges };
	}

	void fn(arg_list args)
	{
		state.assert_net();

		if(args.empty())
		{
			auto neuron_access = state.net->neuron_access();
			auto neurons = neuron_access->get_range(nnet::neuron_filter::HasActivation);
			for(auto const& nr : neurons)
			{
				std::cout << nnet::vis::neuron_name(state.net.get(), nr.id, nnet::vis::NeuronNameStyle::Short);
				std::cout << ": ";
				std::cout << nnet::ActivationFnNames[(int)nr.activation_fn.type];
				std::cout << " (" << nr.activation_fn.steepness << ")" << std::endl;
			}
			return;
		}

		if(args.front() == "random")
		{
			auto dist = std::uniform_int_distribution<> { 0, (int)nnet::ActivationFnType::Count - 1 };

			auto mod = state.net->modifiable();
			auto neuron_access = state.net->neuron_access();
			auto neurons = neuron_access->get_range(nnet::neuron_filter::HasActivation);
			for(auto const& nr : neurons)
			{
				auto const fn_type = (nnet::ActivationFnType)dist(rgen);
				mod->set_activation_fn(nr.id, nnet::activation_function{ fn_type, 1.0 /* TODO: */});
			}
		}
		else
		{
			throw std::runtime_error("Invalid argument");
		}
	}

	void weights(arg_list args)
	{
		state.assert_net();

		if(args.empty())
		{
			auto conn_access = state.net->connection_access();
			auto connections = conn_access->get_range();
			for(auto const& cn : connections)
			{
				std::cout << nnet::vis::neuron_name(state.net.get(), cn.src, nnet::vis::NeuronNameStyle::Short);
				std::cout << " -> ";
				std::cout << nnet::vis::neuron_name(state.net.get(), cn.dst, nnet::vis::NeuronNameStyle::Short);
				std::cout << ": " << cn.weight << std::endl;
			}
			return;
		}

		if(args.front() == "random")
		{
			// TODO:
			auto dist = std::uniform_real_distribution < > { -1.0, 1.0 };

			auto mod = state.net->modifiable();
			auto conn_access = state.net->connection_access();
			auto connections = conn_access->get_range();
			for(auto const& conn : connections)
			{
				auto const weight = dist(rgen);
				mod->set_weight(conn.id, weight);
			}
		}
		else
		{
			throw std::runtime_error("Invalid argument");
		}
	}

	void run(arg_list args)
	{
		state.assert_net();
		state.assert_input();

		auto output = state.net->run(*state.input);
		std::cout << "Running with input: " << *state.input << "..." << std::endl;
		std::cout << output << std::endl;
	}

	void output_ranges(arg_list args)
	{
		state.assert_net();
		state.assert_input_ranges();

		bool visualise = std::find(std::begin(args), std::end(args), "-v") != std::end(args);

		auto neuron_access = state.net->neuron_access();
		auto output_neurons = neuron_access->get_range(nnet::neuron_filter::Output);
		std::set< nnet::neuron_id > output_ids;
		std::transform(
			std::begin(output_neurons),
			std::end(output_neurons),
			std::inserter(output_ids, std::end(output_ids)),
			[](nnet::neuron_data const& nd)
		{
			return nd.id;
		});
		auto output = nnet::calculate_input_output_ranges(
			state.net.get(),
			*state.input_ranges,
			output_ids
			);

		std::cout << "With input ranges of: " << *state.input_ranges << std::endl;
		std::cout << "The output ranges are: ";
		for(auto const& entry : output)
		{
			std::cout << entry.second.output << " ";
		}
		std::cout << std::endl;

		if(visualise)
		{
			range_dd dd;
			std::transform(
				std::begin(output),
				std::end(output),
				std::back_inserter(dd),
				[](nnet::input_output_range_map::value_type const& entry)
			{
				return entry.second.output;
			});
			create_wt_display(display_data{ DisplayType::Range, dd });
		}
	}

	bool execute(Command cmd, arg_list args)
	{
		switch(cmd)
		{
			case Command::exit:
			return false;

			case Command::create:
			create(std::move(args));
			break;

			case Command::load:
			load(std::move(args));
			break;

			case Command::info:
			info(std::move(args));
			break;

			case Command::input:
			input(std::move(args));
			break;

			case Command::input_ranges:
			input_ranges(std::move(args));
			break;

			case Command::fn:
			fn(std::move(args));
			break;

			case Command::weights:
			weights(std::move(args));
			break;

			case Command::run:
			run(std::move(args));
			break;

			case Command::output_ranges:
			output_ranges(std::move(args));
			break;

			case Command::ping:
			std::cout << "pong" << std::endl;
			for(auto const& a : args)
			{
				std::cout << a << std::endl;
			}
			break;

			case Command::server_test:
			//create_wt_display();
			break;
		}

		return true;
	}

}

void prompt()
{
	std::cout << "[";
	if(state.net)
	{
		std::cout << /* TODO:*/ "mlp";
		std::cout << " " <<
			state.net->input_count() << " ";
		auto hidden = state.net->hidden_count();
		if(hidden > 0)
		{
			std::cout << hidden << " ";
		}
		std::cout << state.net->output_count();
	}
	else
	{
		std::cout << "no net";
	}
	std::cout << "]";

	std::cout << "> ";
}

#include "util/multi_interval.h"
namespace bint = boost::numeric;

int _tmain(int argc, _TCHAR* argv[])
{
	std::map< std::string, cmd::Command, str_compare > cmd_map = {
			{ "exit", cmd::Command::exit },
			{ "quit", cmd::Command::exit },
			{ "q", cmd::Command::exit },
			{ "create", cmd::Command::create },
			{ "load", cmd::Command::load },
			{ "info", cmd::Command::info },
			{ "input", cmd::Command::input },
			{ "input_ranges", cmd::Command::input_ranges },
			{ "fn", cmd::Command::fn },
			{ "weights", cmd::Command::weights },
			{ "run", cmd::Command::run },
			{ "output_ranges", cmd::Command::output_ranges },

			{ "ping", cmd::Command::ping },
			{ "server_test", cmd::Command::server_test },
	};

	rgen.seed(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));

	arg_list server_args{
		"nn_cmd.exe",
		"--docroot", ".",
		"--http-address", "0.0.0.0",
		"--http-port", "8080",
		"--accesslog", "NUL"
	};
	
	auto server_ok = start_wt_server(server_args);
	if(!server_ok)
	{
		std::cout << "Failed to start Wt server" << std::endl;
		return 0;
	}

	db_cp.reset(evodb_session::create_connection_pool("../webinterface/evo.db"));
	db_session = std::make_unique< evodb_session >(*db_cp);

	// TODO: Non-portable. Ideally would just disable/redirect initial Wt output
	system("cls");

	while(true)
	{
		prompt();

		std::string in;
		std::getline(std::cin, in);

		auto tokens = bracket_tokenize(in, "\\s+", bracket_set{ { '{', '}' }, { '[', ']' }, { '\"', '\"' } });
		if(tokens.empty())
		{
			continue;
		}

		auto cmd_string = tokens.front();
		auto it = cmd_map.find(cmd_string);
		if(it == cmd_map.end())
		{
			std::cout << "Invalid command" << std::endl;
			continue;
		}

		auto cmd = it->second;
		tokens.pop_front();
		try
		{
			if(!cmd::execute(cmd, std::move(tokens)))
			{
				break;
			}
		}
		catch(std::runtime_error e)
		{
			std::cout << "Error: " << e.what() << std::endl;
		}
	}

	terminate_wt_server();
	return 0;
}

