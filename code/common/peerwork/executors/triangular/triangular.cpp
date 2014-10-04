// triangular.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "triangular.h"

#include <string>
#include <sstream>
#include <thread>
#include <chrono>


extern "C" {

	char const* pw_ex_get_name()
	{
		return "triangular";
	}

	char* pw_ex_run(char const* spec, void* context, pwork::pw_ex_update_cb_fn update_fn, pwork::pw_ex_buffer_alloc_fn alloc)
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
			update_fn(update.c_str(), context);

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		//completion_cb(std::move(result));
		auto buf = alloc(result.length() + 1);
		std::strcpy(buf, result.c_str());
		return buf;
	}

}

