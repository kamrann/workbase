// chart.h

#ifndef __WB_WTDISP_CHART
#define __WB_WTDISP_CHART

#include "display_types.h"
#include "display_base.h"

#include <Wt/WContainerWidget>


class chart_display:
	public Wt::WContainerWidget,
	public display_base
{
public:
	chart_display(chart_dd const& dd);

public:
	virtual void update_from_data(display_data const& dd) override;

private:
	Wt::WStandardItemModel* m_model;
};



#endif


