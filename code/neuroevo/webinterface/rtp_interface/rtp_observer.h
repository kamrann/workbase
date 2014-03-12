// rtp_observer.h

#ifndef __NE_RTP_OBSERVER_H
#define __NE_RTP_OBSERVER_H

#include <boost/any.hpp>

#include <string>
#include <map>


class i_observer
{
public:
	typedef std::map< std::string, boost::any > observations_t;

public:
	virtual void reset() = 0;
};



#endif


