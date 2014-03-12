// rtp_phys_system_drawer.cpp

#include "rtp_phys_system_drawer.h"
#include "rtp_phys_system.h"
#include "rtp_phys_agent_body.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>

#include <Box2D/Box2D.h>

#include <boost/thread/lock_guard.hpp>


namespace rtp_phys {

	phys_system_drawer::phys_system_drawer(phys_system const& sys): m_sys(sys)
	{

	}

	void phys_system_drawer::draw_system(Wt::WPainter& painter)
	{
//		boost::lock_guard< async_system_drawer > guard(*this);

		size_t const Margin = 0;

		Wt::WPaintDevice* device = painter.device();

		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = (size_t)std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		phys_system::state const& st = m_sys.get_state();

		painter.save();

		double const scale = avail_size / 25.0;

		painter.translate(dev_width.toPixels() / 2, dev_height.toPixels() / 2);
		painter.scale(scale, -scale);
		painter.translate(-st.body->get_position().x, -st.body->get_position().y);

		Wt::WPen pen = Wt::WPen(Wt::GlobalColor::black);
		painter.setPen(pen);
		Wt::WBrush br(Wt::GlobalColor::white);
		painter.setBrush(br);

		painter.save();

		// TODO: Not hard coded in
		painter.drawLine(-100.0f, 0.0f, 100.0f, 0.0f);

		st.body->draw(painter);

		painter.restore();
/*
		Wt::WFont font = painter.font();
		//font.setFamily(Wt::WFont::Default);
		font.setSize(20);
		painter.setFont(font);
		pen.setColor(Wt::GlobalColor::blue);
		painter.setPen(pen);
		rc = Wt::WRectF(
			0,
			0,
			dev_width.toPixels(),
			30
			);
		std::stringstream text;
		text.precision(2);
		std::fixed(text);
		text << "v = ";
		text << vector_str< 2, double >(st.ship.lin_velocity);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			30,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "w = ";
		text << st.ship.ang_velocity;
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			60,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "p = ";
		text << vector_str< 2, double >(st.ship.position);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			90,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "o = ";
		text << orientation_str(st.ship.orientation);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());
*/
	}
}



