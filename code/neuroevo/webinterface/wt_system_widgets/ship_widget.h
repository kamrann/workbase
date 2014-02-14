// ship_widget.h

#ifndef __SHIP_AND_THRUSTERS_WIDGET_H
#define __SHIP_AND_THRUSTERS_WIDGET_H

#include "systems/ship_and_thrusters/ship_and_thrusters_system.h"
// TEMP
#include "systems/ship_and_thrusters/scenarios/full_stop.h"
//

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>
#include <Wt/WPen>

#include <boost/math/constants/constants.hpp>


class ship_widget_base: public Wt::WPaintedWidget
{
public:
	// TODO: Move some stuff to a system generic widget base
	enum {
		Margin = 10,
	};
};


template <
	WorldDimensionality Dim_
>
class ship_widget;

// 2D Widget specialization
template <>
class ship_widget< WorldDimensionality::dim2D >: public ship_widget_base
{
public:
	static WorldDimensionality const Dim = WorldDimensionality::dim2D;

	enum {
		LineWidth = 1,
	};

	typedef ship_and_thrusters_system< Dim >	system_t;
	typedef full_stop< Dim >::state				state_t;

public:
	ship_widget()
	{
		m_interaction_enabled = false;
		setLayoutSizeAware(true);
	}

public:
	void enable_interaction(bool enable)
	{
		if(m_interaction_enabled != enable)
		{
			m_interaction_enabled = enable;
		}
	}

	void update_from_system_state(state_t const& st)
	{
		m_st = st;
		ship_widget_base::update();
	}

protected:
	void layoutSizeChanged(int width, int height)
	{
		WPaintedWidget::layoutSizeChanged(width, height);
	}

	void paintEvent(Wt::WPaintDevice* device)
	{
		Wt::WLength dev_width = device->width();
		Wt::WLength dev_height = device->height();
		size_t avail_size = std::min(dev_width.toPixels() - 2 * Margin, dev_height.toPixels() - 2 * Margin);

		Wt::WPainter painter(device);
		Wt::WPen pen(Wt::GlobalColor::black);
		painter.setPen(pen);

		double const scale = avail_size / 5.0;
		painter.scale(scale, scale);
		painter.rotate(m_st.agents[0].orientation * -2 * boost::math::double_constants::pi / 360.0);
		painter.translate(m_st.agents[0].position[0], m_st.agents[0].position[1]);

		double const ShipSize = 1.0;
		Wt::WRectF rc(
			-ShipSize / 2,
			-ShipSize / 2,
			ShipSize,
			ShipSize
			);
		painter.drawRect(rc);
	}

private:
	state_t m_st;	// copy
	bool m_interaction_enabled;
};


#endif


