// peerwork.cpp : Defines the entry point for the console application.
//

#include "peer.h"
#include "executor_loading.h"

#include <boost/program_options.hpp>

#include <iostream>


namespace po = boost::program_options;

/*/////////
void triangular_numbers(
	std::string spec,
	pwork::job_update_callback update_cb,
	pwork::job_completion_callback completion_cb
	)
{
	auto result = std::string{ "" };
	auto num_terms = std::stoul(spec);
	int t_number = 0;
	for(int term = 1; term <= num_terms; ++term)
	{
		t_number += term;
		result += std::to_string(t_number) + " ";

		std::stringstream ss;
		ss << "Term " << term;
		auto update = ss.str();
		update_cb(std::move(update));

		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	completion_cb(std::move(result));
}
/*////////


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
		("name", po::value< std::string >()->default_value("unspecified"), "set peer name")
		("port", po::value< unsigned short >()->default_value(0), "port to listen on")
		("threads", po::value< int >()->default_value(1), "maximum number of worker threads")
		("exec-path", po::value< std::string >(), "peerwork executors path")
		("peers", po::value< std::vector< std::string > >(), "list of peers to look for <ip address>:<port number>")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto peer_name = vm["name"].as< std::string >();
	auto port = vm["port"].as< unsigned short >();
	auto max_threads = vm["threads"].as< int >();
	if(max_threads < 0)
	{
		std::cout << "threads must be >= 0" << std::endl;
		return 0;
	}

	auto exec_path = boost::optional < std::string > {};
	if(vm.count("exec-path"))
	{
		exec_path = vm["exec-path"].as< std::string >();
	}

	std::vector< tcp::endpoint > other_peers;
	if(vm.count("peers"))
	{
		auto peers = vm["peers"].as< std::vector< std::string > >();
		for(auto const& p : peers)
		{
			auto colon = p.find(":");
			if(colon != std::string::npos)
			{
				auto addr = boost::asio::ip::address::from_string(p.substr(0, colon));
				auto port = (unsigned short)std::stoul(p.substr(colon + 1, std::string::npos));

				if(!addr.is_unspecified() && port > 0)
				{
					other_peers.emplace_back(tcp::endpoint{ addr, port });
					continue;
				}
			}

			std::cout << "usage: --peers <ip address>:<port number>..." << std::endl;
			return 0;
		}
	}

	pwork::peer pr{ peer_name, port, max_threads, std::move(other_peers) };

	pwork::executor_mp executors;
	auto num_loaded = pwork::load_all_executors(executors, exec_path);
	for(auto const& entry : executors)
	{
		auto const& name = entry.first;
		auto const& exec = entry.second;

		pr.register_work_type(name, [exec](std::string spec, pwork::job_update_callback update_cb, pwork::job_completion_callback completion_cb)
		{
			auto result = exec->run(spec, update_cb);
			completion_cb(result);
		});
	}

	pr.run();
	return 0;
}

