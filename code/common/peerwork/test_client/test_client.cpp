// test_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../pwork_client/local_peer.h"
#include "../shared/job_description.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <thread>


namespace po = boost::program_options;


int _tmain(int argc, _TCHAR* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "show help message")
		("name", po::value< std::string >()->default_value("unspecified"), "set client name")
		("peer", po::value< std::string >(), "peer location <hostname>:<port number>")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto client_name = vm["name"].as< std::string >();
	//auto peer_location = pwork::find_local_peer();

	if(!vm.count("peer"))
	{
		std::cout << "peer location must be specified" << std::endl;
		return 0;
	}

	auto peer_location = vm["peer"].as< std::string >();
	auto colon = peer_location.find(":");
	std::string hostname, port;
	if(colon != std::string::npos)
	{
		hostname = peer_location.substr(0, colon);
		port = peer_location.substr(colon + 1, std::string::npos);
	}
	else
	{
		std::cout << "usage: --peer <hostname>:<port number>..." << std::endl;
		return 0;
	}

	auto pr = pwork::local_peer::try_connect(client_name, hostname, port);
	if(!pr)
	{
		std::cout << "failed to connect to peer" << std::endl;
		return 0;
	}

	// TODO: This setup probably not thread-safe
	std::thread io_t{ [&pr]{ pr->run(); } };

	while(true)
	{
		std::string cmd;
		std::getline(std::cin, cmd);

		if(cmd == "q")
		{
			break;
		}

		auto space = cmd.find(' ');
		if(space == std::string::npos)
		{
			continue;
		}

		auto type = cmd.substr(0, space);
		if(type == "triangular")
		{
			auto terms = std::stoul(cmd.substr(space + 1, std::string::npos));
			if(terms == 0)
			{
				continue;
			}

			pwork::job_description jd;
			jd.type = type;
			jd.specification = std::to_string(terms);
			pr->post_job(jd);
		}
	}

	pr->disconnect();
	io_t.join();

	return 0;
}

