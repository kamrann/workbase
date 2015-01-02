// drawer.h

#ifndef __WB_WTDISP_RANGE
#define __WB_WTDISP_RANGE

#include "wt_cmdline_server/display_base.h"
#include "wt_cmdline_server/display_defn.h"

#include "system_sim/system_drawer.h"

#include "input_stream/wt_input_stream.h"

#include <Wt/WPaintedWidget>

#include <mutex>
#include <memory>


struct drawer_dd
{
	std::mutex* mx;
	std::shared_ptr< sys::i_system_drawer > drawer;
	double zoom;

	std::shared_ptr< input::wt_input_stream > in_strm;

	drawer_dd():
		mx{ nullptr },
		drawer{ nullptr },
		zoom{ 1.0 }
	{}
};

class drawer_display:
	public Wt::WPaintedWidget,
	public display_base
{
public:
	drawer_display(drawer_dd const& dd);

public:
	virtual Wt::WWidget* get_wt_widget() override;
	virtual void update_from_data(boost::any const& dd) override;

protected:
	void paintEvent(Wt::WPaintDevice* device);

private:
	drawer_dd m_dd;
};


class drawer_display_defn:
	public display_defn
{
public:
	static const std::string ID;

	virtual std::string name() const override;
	virtual display_base* create(boost::any const& data) const override;
};



#endif


