// display_base.h

#ifndef __WB_WTDISP_DISPLAY_BASE
#define __WB_WTDISP_DISPLAY_BASE

#include "display_types.h"


class display_base
{
public:
	virtual void update_from_data(display_data const& dd) = 0;
};



#endif


