// ship_widget.h

#ifndef __SHIP_AND_THRUSTERS_WIDGET_H
#define __SHIP_AND_THRUSTERS_WIDGET_H

#include "../rtp_interface/systems/rtp_system_drawer.h"

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>


class ship_widget: public Wt::WPaintedWidget
{
public:
	enum {
		Margin = 10,
		LineWidth = 1,
	};

public:
	ship_widget(): m_sys_drawer(nullptr)//: m_st(nullptr)
	{
		m_interaction_enabled = false;
		setLayoutSizeAware(true);

		setAttributeValue("tabindex", "0");

		keyWentDown().connect([this](Wt::WKeyEvent e)
		{
			if(!m_interaction_enabled)
			{
				return;
			}

/*			int const base = 0x31;
			if(e.key() >= base && e.key() < base + 8)
			{
				size_t idx = e.key() - base;
				m_thruster_signal.emit(idx, true);
			}
			*/
			size_t idx = 0;
			switch(e.key())
			{
			case Wt::Key_Insert:		idx = 0;	break;
			case Wt::Key_Delete:		idx = 1;	break;
			case Wt::Key_Home:			idx = 2;	break;
			case Wt::Key_End:			idx = 3;	break;
			case Wt::Key_PageUp:		idx = 4;	break;
			case Wt::Key_PageDown:		idx = 5;	break;
			default:
				return;
			}
			m_thruster_signal.emit(idx, true);
		}
		);

		keyWentUp().connect([this](Wt::WKeyEvent e)
		{
			if(!m_interaction_enabled)
			{
				return;
			}
			/*
			int const base = 0x31;
			if(e.key() >= base && e.key() < base + 8)
			{
				size_t idx = e.key() - base;
				m_thruster_signal.emit(idx, false);
			}
			*/
			size_t idx = 0;
			switch(e.key())
			{
			case Wt::Key_Insert:		idx = 0;	break;
			case Wt::Key_Delete:		idx = 1;	break;
			case Wt::Key_Home:			idx = 2;	break;
			case Wt::Key_End:			idx = 3;	break;
			case Wt::Key_PageUp:		idx = 4;	break;
			case Wt::Key_PageDown:		idx = 5;	break;
			default:
				return;
			}
			m_thruster_signal.emit(idx, false);
		}
		);
	}

public:
	void set_drawer(i_system_drawer* drawer)
	{
		m_sys_drawer = drawer;
	}

	void enable_interaction(bool enable)
	{
		if(m_interaction_enabled != enable)
		{
			m_interaction_enabled = enable;
		}
	}
	/*
	void update_from_system_state()//state_t* st)
	{
//		m_st = boost::shared_ptr< state_t >(st);
		ship_widget_base::update();
	}
	*/

	Wt::Signal< size_t, bool >& thruster_activated()
	{
		return m_thruster_signal;
	}

protected:
	void layoutSizeChanged(int width, int height)
	{
		WPaintedWidget::layoutSizeChanged(width, height);
	}

	void paintEvent(Wt::WPaintDevice* device)
	{
		if(m_sys_drawer != nullptr)
		{
			Wt::WPainter painter(device);
			m_sys_drawer->draw_system(painter);
		}

/*		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = (size_t)std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		Wt::WPainter painter(device);
		painter.save();

		Wt::WPen pen(Wt::GlobalColor::lightGray);
		painter.setPen(pen);

		double const scale = avail_size / 10.0;

		size_t const GridDim = 5;
		double const GridSquareSize = avail_size / GridDim;

		double x_off = std::fmod(-m_st->get_ship_state(0).position[0] * scale, GridSquareSize);
		if(x_off < 0.0)
		{
			x_off += GridSquareSize;
		}
		double y_off = std::fmod(-m_st->get_ship_state(0).position[1] * -scale, GridSquareSize);
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

		//painter.translate(m_st.agents[0].position[0], m_st.agents[0].position[1]);
		painter.rotate(m_st->get_ship_state(0).orientation * 360.0 / (2 * boost::math::double_constants::pi));

		pen = Wt::WPen(Wt::GlobalColor::black);
		painter.setPen(pen);
		Wt::WBrush br(Wt::GlobalColor::white);
		painter.setBrush(br);
		double const ShipSize = 2.0;
		Wt::WRectF rc(
			-ShipSize / 2,
			-ShipSize / 2,
			ShipSize,
			ShipSize
			);
		painter.drawRect(rc);

		double const ThrusterSize = ShipSize * 0.15;
		std::array< Wt::WPointF, 3 > points = {
			WPointF(0, 0),
			WPointF(-ThrusterSize / 2, -ThrusterSize),
			WPointF(ThrusterSize / 2, -ThrusterSize)
		};
		for(size_t i = 0; i < m_st->get_ship_state(0).thruster_cfg->num_thrusters(); ++i)
		{
			painter.save();

			auto const& tpos = m_st->get_ship_thruster_sys(0)[i].pos;
			painter.translate(tpos[0], tpos[1]);
			double dir_x = m_st->get_ship_thruster_sys(0)[i].dir[0];
			double dir_y = m_st->get_ship_thruster_sys(0)[i].dir[1];
			double theta = boost::math::copysign(std::acos(dir_y), dir_x);
			painter.rotate(360.0 * -theta / (2 * boost::math::double_constants::pi));

			double temperature = m_st->get_ship_thruster_sys(0)[i].t.temperature();
			Wt::WColor color((int)(temperature * 255), 0, 0);
			pen.setColor(color);
			painter.setPen(pen);
			Wt::WBrush br(color);
			painter.setBrush(br);
			painter.drawPolygon(&points[0], points.size());

			painter.restore();
		}

		painter.restore();
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
		text << vector_str< 2, double >(m_st->get_ship_state(0).lin_velocity);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			30,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "w = ";
		text << m_st->get_ship_state(0).ang_velocity;
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			60,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "p = ";
		text << vector_str< 2, double >(m_st->get_ship_state(0).position);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());

		rc = Wt::WRectF(
			0,
			90,
			dev_width.toPixels(),
			30
			);
		text.str("");
		text << "o = ";
		text << orientation_str(m_st->get_ship_state(0).orientation);
		painter.drawText(rc, Wt::AlignLeft | Wt::AlignMiddle, text.str());
*/
	}

private:
	i_system_drawer* m_sys_drawer;
	bool m_interaction_enabled;
	Wt::Signal< size_t, bool > m_thruster_signal;
};


#endif


