// rtp_resultant_objective.h

#ifndef __NE_RTP_RESULTANT_OBJECTIVE_H
#define __NE_RTP_RESULTANT_OBJECTIVE_H

#include "rtp_observer.h"
#include "rtp_pop_wide_observer.h"

#include "wt_param_widgets/pw_fwd.h"


namespace rtp
{
	class resultant_objective
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);

	public:
		static std::unique_ptr< i_population_wide_observer > create_instance(
			prm::param_accessor param,
			std::vector< std::unique_ptr< i_observer > >& required_observations);
	};
}


#endif

