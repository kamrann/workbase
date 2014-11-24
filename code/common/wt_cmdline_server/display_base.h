// display_base.h

#ifndef __WB_WTDISP_DISPLAY_BASE
#define __WB_WTDISP_DISPLAY_BASE

#include <boost/any.hpp>


namespace Wt {
	class WWidget;
}

class display_base
{
public:
	virtual Wt::WWidget* get_wt_widget() = 0;
	virtual void update_from_data(boost::any const& dd) = 0;
};



#endif


