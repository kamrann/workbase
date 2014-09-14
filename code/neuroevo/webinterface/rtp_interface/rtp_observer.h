// rtp_observer.h

#ifndef __NE_RTP_OBSERVER_H
#define __NE_RTP_OBSERVER_H

#include <boost/any.hpp>

#include <string>
#include <map>


namespace rtp {

	class i_system;

	class i_observer
	{
	public:
		typedef std::map< std::string, boost::any > observations_t;
		typedef boost::any trial_data_t;

	public:
		virtual void update(i_system const* system) = 0;
		virtual void reset() = 0;
		virtual observations_t record_observations(trial_data_t const& td) = 0;

	public:
		virtual ~i_observer() {}
	};

}


#endif


