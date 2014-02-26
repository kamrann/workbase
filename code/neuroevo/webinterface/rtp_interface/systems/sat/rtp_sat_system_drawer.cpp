// rtp_sat_system_drawer.cpp

#include "rtp_sat_system_drawer.h"
#include "rtp_sat_system.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>


namespace rtp_sat {

	template < WorldDimensionality dim >
	sat_system_drawer< dim >::sat_system_drawer(sat_system< dim > const& sys): m_sys(sys)
	{

	}

	template < WorldDimensionality dim >
	void sat_system_drawer< dim >::draw_system(Wt::WPainter& painter)
	{
		size_t const Margin = 0;

		Wt::WPaintDevice* device = painter.device();

		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = (size_t)std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		sat_system< dim >::state const& st = m_sys.get_state();

		painter.save();

		Wt::WPen pen(Wt::GlobalColor::lightGray);
		painter.setPen(pen);

		double const scale = avail_size / 10.0;

		size_t const GridDim = 5;
		double const GridSquareSize = avail_size / GridDim;

		double x_off = std::fmod(-st.ship.position[0] * scale, GridSquareSize);
		if(x_off < 0.0)
		{
			x_off += GridSquareSize;
		}
		double y_off = std::fmod(-st.ship.position[1] * -scale, GridSquareSize);
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
		painter.rotate(st.ship.orientation * 360.0 / (2 * boost::math::double_constants::pi));

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
			Wt::WPointF(0, 0),
			Wt::WPointF(-ThrusterSize / 2, -ThrusterSize),
			Wt::WPointF(ThrusterSize / 2, -ThrusterSize)
		};
		for(size_t i = 0; i < st.ship.thruster_cfg->num_thrusters(); ++i)
		{
			painter.save();

			auto const& tpos = st.thrusters[i].pos;
			painter.translate(tpos[0], tpos[1]);
			double dir_x = st.thrusters[i].dir[0];
			double dir_y = st.thrusters[i].dir[1];
			double theta = boost::math::copysign(std::acos(dir_y), dir_x);
			painter.rotate(360.0 * -theta / (2 * boost::math::double_constants::pi));

			double temperature = st.thrusters[i].t.temperature();
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
	}

	template class sat_system_drawer< WorldDimensionality::dim2D >;
	//template class sat_system_drawer< WorldDimensionality::dim3D >;
}



