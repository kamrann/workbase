// rtp_phys_system_drawer.cpp

#include "rtp_phys_system_drawer.h"
#include "rtp_phys_system.h"
#include "rtp_phys_scenario.h"
#include "rtp_phys_agent_body.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>

#include <Box2D/Box2D.h>

#include <boost/thread/lock_guard.hpp>


namespace rtp {

	phys_system_drawer::phys_system_drawer(phys_system const& sys): m_sys(sys)
	{

	}

	void phys_system_drawer::draw_system(Wt::WPainter& painter, options_t const& options)
	{
//		boost::lock_guard< async_system_drawer > guard(*this);

		size_t const Margin = 0;

		Wt::WPaintDevice* device = painter.device();

		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = (size_t)std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		phys_system::state const& st = m_sys.get_state();

		painter.save();

		Wt::WPen pen(Wt::GlobalColor::lightGray);
		painter.setPen(pen);

		double const scale = (avail_size / 25.0) * options.zoom;

		size_t const GridDim = 5;
		double const GridSquareSize = avail_size / GridDim;

		// TODO: Hack - locking onto first agent
		b2Vec2 grid_ref_pos = m_sys.m_agents.front().agent->get_position();	//b2Vec2(0, 0);

		double x_off = std::fmod(-grid_ref_pos.x * scale, GridSquareSize);
		if(x_off < 0.0)
		{
			x_off += GridSquareSize;
		}
		double y_off = std::fmod(-grid_ref_pos.y * -scale, GridSquareSize);
		if(y_off < 0.0)
		{
			y_off += GridSquareSize;
		}

		for(size_t i = 0; i < dev_width.toPixels() / GridSquareSize; ++i)
		{
			painter.drawLine(
				x_off + i * GridSquareSize,
				0.0,
				x_off + i * GridSquareSize,
				dev_height.toPixels()
				);
		}

		for(size_t i = 0; i < dev_height.toPixels() / GridSquareSize; ++i)
		{
			painter.drawLine(
				0.0,
				y_off + i * GridSquareSize,
				dev_width.toPixels(),
				y_off + i * GridSquareSize
				);
		}

		painter.translate(dev_width.toPixels() / 2, dev_height.toPixels() / 2);
		painter.scale(scale, -scale);
		painter.translate(-grid_ref_pos.x, -grid_ref_pos.y);

		pen = Wt::WPen(Wt::GlobalColor::black);
		painter.setPen(pen);
		Wt::WBrush br(Wt::GlobalColor::white);
		painter.setBrush(br);

		painter.save();

		m_sys.m_scenario->draw_fixed_objects(painter);

		//phys_system::const_agent_range agents = m_sys.get_agent_range();
		//for(auto it = agents.first; it != agents.second; ++it)
		for(auto const& agent: m_sys.m_agents)
		{
			//(*it)->draw(painter);
			agent.agent->draw(painter);
		}

		painter.restore();
		painter.restore();
		
		Wt::WFont font = painter.font();
		//font.setFamily(Wt::WFont::Default);
		font.setSize(20);
		painter.setFont(font);
		pen.setColor(Wt::GlobalColor::blue);
		painter.setPen(pen);
		auto rc = Wt::WRectF(
			0,
			0,
			dev_width.toPixels(),
			30
			);
		std::stringstream text;
		text.precision(2);
		std::fixed(text);
		text << m_sys.get_state().time << "s";
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());
/*
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



