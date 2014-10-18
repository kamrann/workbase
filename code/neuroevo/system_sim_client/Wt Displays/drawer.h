// drawer.h

#ifndef __WB_WTDISP_RANGE
#define __WB_WTDISP_RANGE

#include "display_types.h"
#include "display_base.h"

#include <Wt/WPaintedWidget>


class drawer_display:
	public Wt::WPaintedWidget,
	public display_base
{
public:
	drawer_display(drawer_dd const& dd);

public:
	virtual void update_from_data(display_data const& dd) override;

protected:
	void paintEvent(Wt::WPaintDevice* device);

private:
	drawer_dd m_dd;
};



#endif


