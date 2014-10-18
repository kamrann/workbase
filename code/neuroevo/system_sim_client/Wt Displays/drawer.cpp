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

void drawer_display::update_from_data(display_data const& dd)
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


