// job_function_defns.h

#ifndef __WB_PWORK_JOB_FUNCTION_DEFNS_H
#define __WB_PWORK_JOB_FUNCTION_DEFNS_H

#include <string>
#include <functional>


namespace pwork {

	typedef std::function< void(std::string) > job_update_callback;
	typedef std::function< void(std::string) > job_completion_callback;
	typedef std::function< void(std::string, job_update_callback, job_completion_callback) > peerwork_job_fn;

}


#endif


