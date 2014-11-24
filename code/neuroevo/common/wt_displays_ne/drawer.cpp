// drawer.cpp

#include "drawer.h"
#include "system_sim/system_drawer.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>


drawer_display::drawer_display(drawer_dd const& dd):
m_dd(dd)
{
	resize(400, 400);	// TODO:
}

Wt::WWidget* drawer_display::get_wt_widget()
{
	return this;
}

void drawer_display::update_from_data(boost::any const& dd)
{
	update();
}

void drawer_display::paintEvent(Wt::WPaintDevice* device)
{
	auto dev_width = device->width().toPixels();
	auto dev_height = device->height().toPixels();

	Wt::WPainter painter(device);

	std::lock_guard< std::mutex > lock_{ *m_dd.mx };
	m_dd.drawer->draw_system(painter);
}


// TODO: these need to be non-clashing, perhaps use UUID
const std::string drawer_display_defn::ID = "sys_drawer";

std::string drawer_display_defn::name() const
{
	return ID;
}

display_base* drawer_display_defn::create(boost::any const& data) const
{
	return new drawer_display{ boost::any_cast<drawer_dd const&>(data) };
}


