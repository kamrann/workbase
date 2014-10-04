// job_description.h

#ifndef __WB_PWORK_JOB_DESCRIPTION_H
#define __WB_PWORK_JOB_DESCRIPTION_H

#include <string>


namespace pwork {

	// Describes the work to be done
	struct job_description
	{
		std::string type;
		std::string specification;
	};

}



#endif


