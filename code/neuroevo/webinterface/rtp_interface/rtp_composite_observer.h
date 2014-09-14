// rtp_composite_observer.h

#ifndef __NE_RTP_COMPOSITE_OBSERVER_H
#define __NE_RTP_COMPOSITE_OBSERVER_H

#include "rtp_observer.h"

#include <vector>
#include <memory>


namespace rtp
{
	class composite_observer: public i_observer
	{
	public:
		static std::unique_ptr< i_observer > create_instance(
			std::vector< std::unique_ptr< i_observer > > required_observations);

	public:
		virtual void reset() override;
		virtual void update(i_system const* system) override;
		virtual i_observer::observations_t record_observations(trial_data_t const& td) override;

	protected:
		std::map< std::string, std::unique_ptr< i_observer > > m_single_value_observers;
	};
}


#endif

