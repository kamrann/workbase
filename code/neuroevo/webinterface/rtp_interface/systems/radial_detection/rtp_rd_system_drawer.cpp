// rtp_rd_system_drawer.cpp

#include "rtp_rd_system_drawer.h"
#include "rtp_radial_detection_system.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>

#include <boost/thread/lock_guard.hpp>


namespace rtp {

	rd_system_drawer::rd_system_drawer(rd_system const& sys): m_sys(sys)
	{

	}

	void rd_system_drawer::draw_system(Wt::WPainter& painter, options_t const& options)
	{
//		boost::lock_guard< async_system_drawer > guard(*this);

		size_t const Margin = 0;

		Wt::WPaintDevice* device = painter.device();

		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = (size_t)std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		painter.save();

		double const scale = (avail_size / 3.0) * options.zoom;

		painter.translate(dev_width.toPixels() / 2, dev_height.toPixels() / 2);
		painter.scale(scale, -scale);
		//painter.translate(-grid_ref_pos.x, -grid_ref_pos.y);

		Wt::WPen pen = Wt::WPen(Wt::GlobalColor::black);
		painter.setPen(pen);
		Wt::WBrush br(Wt::GlobalColor::white);
		painter.setBrush(br);

		painter.save();

		Wt::WRectF rc_obj{
			m_sys.m_object.position[0] - m_sys.m_object.radius,
			m_sys.m_object.position[1] - m_sys.m_object.radius,
			m_sys.m_object.radius * 2.0,
			m_sys.m_object.radius * 2.0
		};
		painter.drawEllipse(rc_obj);

		auto const vel_line_length = 0.2;
		painter.setPen(Wt::WPen(Wt::red));
		painter.drawLine(
			m_sys.m_object.position[0],
			m_sys.m_object.position[1],
			m_sys.m_object.position[0] + m_sys.m_object.velocity[0] * vel_line_length,
			m_sys.m_object.position[1] + m_sys.m_object.velocity[1] * vel_line_length
			);

		for(size_t i = 0; i < m_sys.m_num_sensors; ++i)
		{
			if(m_sys.m_sensor_readings[i].distance <= 0.0)
			{
				continue;
			}

			auto const bearing = ((double)i / m_sys.m_num_sensors) * 2.0 * M_PI;
			auto const sensor_dir = rd_system::dim_traits::bearing_to_vector(bearing);
			auto const lateral_dir = rd_system::dim_traits::direction_t{ sensor_dir[1], -sensor_dir[0] };

			painter.setPen(Wt::WPen(Wt::green));
			painter.drawLine(
				0.0,
				0.0,
				0.0 + sensor_dir[0] * m_sys.m_sensor_readings[i].distance,
				0.0 + sensor_dir[1] * m_sys.m_sensor_readings[i].distance
				);

			painter.setPen(Wt::WPen(Wt::red));
			auto const base_pos = rd_system::dim_traits::position_t{ 0, 0 } +
				sensor_dir * m_sys.m_sensor_readings[i].distance * 0.5 +
				lateral_dir * 0.1;
			painter.drawLine(
				base_pos[0],
				base_pos[1],
				base_pos[0] + sensor_dir[0] * vel_line_length * m_sys.m_sensor_readings[i].radial_vel,
				base_pos[1] + sensor_dir[1] * vel_line_length * m_sys.m_sensor_readings[i].radial_vel
				);
			painter.drawLine(
				base_pos[0],
				base_pos[1],
				base_pos[0] - lateral_dir[0] * vel_line_length * m_sys.m_sensor_readings[i].lateral_vel,
				base_pos[1] - lateral_dir[1] * vel_line_length * m_sys.m_sensor_readings[i].lateral_vel
				);
		}

		auto const decision_vec = rd_system::dim_traits::bearing_to_vector(m_sys.m_decision.angle);
		painter.setPen(Wt::WPen(Wt::blue));
		painter.drawLine(
			0.0,
			0.0,
			0.0 + decision_vec[0] * 0.3,
			0.0 + decision_vec[1] * 0.3
			);

		auto const agent_radius = 0.05;
		Wt::WRectF rc_agent{
			-agent_radius,
			-agent_radius,
			agent_radius * 2.0,
			agent_radius * 2.0
		};
		painter.setPen(Wt::WPen(Wt::black));
		painter.setBrush(Wt::WBrush(Wt::black));
		painter.drawEllipse(rc_agent);

		painter.restore();
		painter.restore();
	
		/*
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
		*/
	}
}



