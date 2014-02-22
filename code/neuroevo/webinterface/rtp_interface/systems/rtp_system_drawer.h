// rtp_system_drawer.h

#ifndef __NE_RTP_SYSTEM_DRAWER_H
#define __NE_RTP_SYSTEM_DRAWER_H


namespace Wt {
	class WPainter;
}

class i_system_drawer
{
public:
	virtual void draw_system(Wt::WPainter& painter) = 0;
};


#endif

