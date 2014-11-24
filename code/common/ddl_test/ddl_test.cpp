// ddl_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ddl/ddl.h"
#include "ddl/output.h"
#include "ddl/values/navigator.h"
#include "ddl/state_machine/paramtree_fsm.h"

#include "problem_domain_ddl.h"

#include "wt_cmdline_server/wt_server.h"

#include <boost/program_options.hpp>

#include <thread>
#include <chrono>


namespace po = boost::program_options;
using namespace ddl;


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	arg_list server_args{
		"ddl_test.exe",
		"--docroot", ".",
		"--http-address", "0.0.0.0",
		"--http-port", "8080"
	};

	auto server_ok = start_wt_server(server_args);
	if(!server_ok)
	{
		std::cout << "Failed to start Wt server" << std::endl;
		return 0;
	}

	auto term = create_wt_terminal("ddl%20test%20terminal");

	auto sink = [term](std::string text)
	{
		term->output(text);
	};

	auto prompt_cb = [term](std::string prompt)
	{
		term->set_prompt(prompt);
	};


	specifier spc;
	problem_domain_schema schema;
	auto defn = schema.get_defn(spc);

	fsm::paramtree_editor ed{
		defn,
		sink
	};

	term->register_command_handler([&ed, term](std::string cmd)
	{
		// NOTE: This handler is invoked from a Wt session thread
		std::cout << "cmd [" << cmd << "]" << std::endl;

		ed.process_event(clsm::ev_unprocessed_cmd_str{ cmd });

		if(ed.terminated())
		{
			
		}
		else
		{
			term->set_prompt(ed.get_prompt());
		}
	});

	ed.initiate();

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	terminate_wt_server();
	return 0;
}







#if 0

int _tmain(int argc, _TCHAR* argv[])
{
/*	auto value_obj = composite();
	value_obj.add("direction", enumeration("maximise")("minimise"));
	value_obj.add("value", enumeration(ext("state_values")));
	value_obj.add("type", enumeration("value")("average")("sum")("minimum")("maximum"));

	auto objective = composite();
	objective.add("type", enumeration("single", "pareto"));
	objective.add("content", conditional
		(eq("type", "single"), value_obj)	// TODO: option to use node ptr (return value of above add()?) instead of text for "type"?
		(eq("type", "pareto"), list(at_least(1), value_obj))
		);

	instead of as below, think externals should be bound to functions at the instantiate step, since the functions
	will generally be dependent upon the values, which are only available at that time.
	that said, may not be necessary if functions are of form fn(value_map)...

	auto get_state_values = []
	{
		return std::vector < std::string > { "svA", "svB", "svC" };
	};

	auto test = composite();
	test.add("objective", objective(set_ext("state_values", enum_values_fn(get_state_values))));
*/

	
	objective_schema schema;
	auto defn = schema.get_defn();	

	/////////////////////////////////////////

	ref_resolver rr = [](node_ref const& ref, navigator nav) -> value_node
	{
		if(nav)
		{
			auto id = ref.nd.get_id();
			auto results = nav.find_by_id(id);
			// TODO:
			return results.by_location.begin()->second.front();
		}
		else
		{
			return{};
		}
	};

	value_node vals;
	auto sch_nd = instantiate(defn, navigator{ vals }, rr);
//	std::cout << "Schema:" << std::endl << sch_nd;

	adjust_to_schema(vals, sch_nd);
	std::cout << "Defaults:" << std::endl << vals;

	auto nav = navigator{ vals };
//	auto type = nav[node_name{ "type" }].get();
//	type = enum_sch_node::value_t{ std::string{ "single" } };
	auto lst = nav.get();
	auto items = lst.as_list();
	items.push_back(value_node{});
	items.push_back(value_node{});
	lst = items;

	sch_nd = instantiate(defn, navigator{ vals }, rr);
	adjust_to_schema(vals, sch_nd);
	std::cout << "Defaults:" << std::endl << vals;


/*	nav = nav["temp"];
	auto int_nd = nav.get();
	int_nd = 20.0;
	std::cout << "Defaults:" << std::endl << vals;
*/
	/*
	test_inst["objective"]["type"] = "pareto";
	test_inst["objective"]["content"].add();
	test_inst["objective"]["content"][0]["direction"] = "minimise";
	*/
	return 0;
}

#endif


