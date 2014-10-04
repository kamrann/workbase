// executor_loading.h

#ifndef __WB_PWORK_EXECUTOR_LOADING_H
#define __WB_PWORK_EXECUTOR_LOADING_H

#include "job_function_defns.h"

#include <boost/optional.hpp>

#include <string>
#include <memory>
#include <functional>
#include <map>


namespace pwork {

	typedef char* (*pw_ex_buffer_alloc_fn)(size_t);
	typedef char const* (*pw_ex_get_name_fn)();
	typedef void (*pw_ex_update_cb_fn)(char const* update, void* context);
	typedef char* (*pw_ex_run_fn)(char const* spec, void* context, pw_ex_update_cb_fn update_fn, pw_ex_buffer_alloc_fn alloc);


	class executor
	{
	public:
		static std::shared_ptr< executor > load(std::string filename);

		std::string get_name() const;
		std::string run(std::string specification, job_update_callback update_cb) const;

		~executor();

	private:
		struct ctr_key {};

	public:
		executor(std::string _name, pw_ex_run_fn _run_fn, std::function< void() > _release_fn, ctr_key);

	private:
		executor(executor const&) = delete;
		executor& operator= (executor const&) = delete;

	private:
		static void update_callback(char const* update, void* context);
		static char* allocate_buffer(size_t size);
		static void free_buffer(char* buffer);

	private:
		std::string m_name;
		//pw_ex_run_fn m_run_fn;
		std::function< char*(char const* spec, void* context, pw_ex_update_cb_fn update_fn, pw_ex_buffer_alloc_fn alloc) > m_run_fn;
		std::function< void() > m_release_fn;

		static char const* const s_get_name_fn_name;
		static char const* const s_run_fn_name;
	};


	typedef std::map< std::string, std::shared_ptr< executor > > executor_mp;

	size_t load_all_executors(executor_mp& executors, boost::optional< std::string > path = boost::none);


}



#endif


