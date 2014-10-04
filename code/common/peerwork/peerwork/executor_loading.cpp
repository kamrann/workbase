// executor_loading.cpp

#include "executor_loading.h"
#include "peer.h"

#include <Windows.h>

#include <iostream>
#include <filesystem>
#include <regex>


namespace pwork {

	char const* const executor::s_get_name_fn_name = "pw_ex_get_name";
	char const* const executor::s_run_fn_name = "pw_ex_run";


	std::shared_ptr< executor > executor::load(std::string filename)
	{
		auto hDllInst = ::LoadLibrary(filename.c_str());
		if(!hDllInst)
		{
			std::cout << "could not load executor: " << filename << std::endl;
			return nullptr;
		}

		auto get_name_fn = reinterpret_cast< pw_ex_get_name_fn >(::GetProcAddress(hDllInst, s_get_name_fn_name));
		if(!get_name_fn)
		{
			std::cout << "could not locate executor function: " << s_get_name_fn_name << std::endl;
			::FreeLibrary(hDllInst);
			return nullptr;
		}

		auto run_fn = reinterpret_cast< pw_ex_run_fn >(::GetProcAddress(hDllInst, s_run_fn_name));
		if(!run_fn)
		{
			std::cout << "could not locate executor function: " << s_run_fn_name << std::endl;
			::FreeLibrary(hDllInst);
			return nullptr;
		}

		return std::make_shared< executor >(get_name_fn(), run_fn, [hDllInst]{ ::FreeLibrary(hDllInst); }, ctr_key{});
	}

	std::string executor::get_name() const
	{
		return m_name;
	}

	std::string executor::run(std::string specification, job_update_callback update_cb) const
	{
		void* context = &update_cb;
		auto result_buf = m_run_fn(specification.c_str(), context, &update_callback, &allocate_buffer);
		auto result = std::string{ result_buf };
		free_buffer(result_buf);
		return result;
	}

	executor::executor(std::string _name, pw_ex_run_fn _run_fn, std::function< void() > _release_fn, ctr_key):
		m_name{ _name },
		m_run_fn{ _run_fn },
		m_release_fn{ _release_fn }
	{}

	executor::~executor()
	{
		if(m_release_fn)
		{
			m_release_fn();
		}
	}

	void executor::update_callback(char const* update, void* context)
	{
		auto update_ftr_ptr = static_cast<job_update_callback*>(context);
		(*update_ftr_ptr)(std::string{ update });
	}

	char* executor::allocate_buffer(size_t size)
	{
		return new char[size];
	}

	void executor::free_buffer(char* buffer)
	{
		delete buffer;
	}


	namespace fs = std::tr2::sys;

	size_t load_all_executors(executor_mp& executors, boost::optional< std::string > path)
	{
		size_t num_loaded = 0;

		auto filter_str = std::string(R"(.*\.pwex)");
		std::regex rgx(filter_str);

		auto fs_path = path ? fs::path{ *path } : fs::current_path< fs::path >();
		for(auto it = fs::directory_iterator(fs_path); it != fs::directory_iterator(); ++it)
		{
			auto const& file = it->path();

			if(!fs::is_directory(file) && std::regex_search(file.filename(), rgx))
			{
				auto exec = executor::load(file);
				if(exec)
				{
					executors[exec->get_name()] = exec;
					++num_loaded;
				}
			}
		}

		if(num_loaded > 0)
		{
			std::cout << "successfully loaded " << num_loaded << " executors" << std::endl;
		}
		else
		{
			std::cout << "warning: failed to load any executors" << std::endl;
		}

		return num_loaded;
	}

}


