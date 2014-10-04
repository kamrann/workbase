// local_job.h

#ifndef __WB_PWORK_LOCAL_JOB_H
#define __WB_PWORK_LOCAL_JOB_H

#include "../shared/job_id.h"
#include "../shared/job_description.h"
#include "client_id.h"
#include "peer_id.h"

#include <boost/optional.hpp>

#include <chrono>


namespace pwork {

	struct local_job
	{
		job_id id;
		job_description desc;
		client_id client;

		boost::optional< peer_id > executing_peer;
		std::chrono::system_clock::time_point last_ping;
	};

}


#endif


