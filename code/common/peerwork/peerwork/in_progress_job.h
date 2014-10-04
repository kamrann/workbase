// in_progress_job.h

#ifndef __WB_PWORK_IN_PROGRESS_JOB_H
#define __WB_PWORK_IN_PROGRESS_JOB_H

#include "peer_id.h"
#include "../shared/job_id.h"
#include "../shared/job_description.h"


namespace pwork {

	struct in_progress_job
	{
		peer_id pid;
		job_id remote_id;	// Id of the job on the owning peer
		job_description desc;	// Needed?
	};

}


#endif


