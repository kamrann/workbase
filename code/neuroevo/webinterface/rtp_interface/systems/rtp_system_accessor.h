// rtp_system_accessor.h

#ifndef __NE_RTP_SYSTEM_ACCESSOR_H
#define __NE_RTP_SYSTEM_ACCESSOR_H


#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>


template < typename SystemT >
class system_accessor: public boost::basic_lockable_adapter< boost::mutex >
{
public:
	system_accessor(SystemT& sys): m_sys(sys)
	{}

	operator SystemT& ()
	{
		return m_sys;
	}

	operator SystemT const& () const
	{
		return m_sys;
	}

protected:
	SystemT& m_sys;
};



#endif

