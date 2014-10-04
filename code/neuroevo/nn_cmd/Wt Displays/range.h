// range.h

#ifndef __WB_NNCMD_WTDISP_RANGE
#define __WB_NNCMD_WTDISP_RANGE

#include "display_types.h"

#include <Wt/WContainerWidget>


class range_display:
	public Wt::WContainerWidget
{
public:
	range_display(range_dd const& dd);
};



#endif


