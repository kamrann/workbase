// rtp_system_drawer.h

#ifndef __NE_RTP_SYSTEM_DRAWER_H
#define __NE_RTP_SYSTEM_DRAWER_H

#include <boost/thread/mutex.hpp>


namespace Wt {
	class WPainter;
}

class i_system_drawer
{
public:
	virtual void draw_system(Wt::WPainter& painter) = 0;
};

/*
class async_system_drawer
{
public:
	async_system_drawer(): m_mtx(nullptr)
	{}

	void set_mutex(boost::mutex* mtx)
	{
		m_mtx = mtx;
	}

	void lock()
	{
		if(m_mtx)
		{
			m_mtx->lock();
		}
	}

	void unlock()
	{
		if(m_mtx)
		{
			m_mtx->unlock();
		}
	}

private:
	boost::mutex* m_mtx;
};
*/

#endif

