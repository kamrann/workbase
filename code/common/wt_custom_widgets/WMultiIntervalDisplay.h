// WMultiIntervalDisplay.h

#ifndef __WB_WTCUSTOM_MULTIINTERVALDISPLAY_H
#define __WB_WTCUSTOM_MULTIINTERVALDISPLAY_H

#include "util/multi_interval.h"

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>


template < typename MI >
class WMultiIntervalDisplay;

template < typename T, typename Policies, template < typename, typename > class Approximator >
class WMultiIntervalDisplay< multi_interval< T, Policies, Approximator > >:
	public Wt::WPaintedWidget
{
public:
	typedef multi_interval< T, Policies, Approximator > multi_interval_t;

public:
	WMultiIntervalDisplay():
		m_mi{}
	{
		resize(200, 40);
	}

public:
	void set_multi_interval(multi_interval_t const& mi)
	{
		m_mi = mi;
		update();
	}

protected:
	void paintEvent(Wt::WPaintDevice* device)
	{
		auto dev_width = device->width().toPixels();
		auto dev_height = device->height().toPixels();

		Wt::WPainter painter(device);
		painter.setPen(Wt::WPen{ Wt::black });
		painter.setBrush(Wt::WBrush{ Wt::WColor{ 150, 150, 150 } });

		if(m_mi.is_empty())
		{
			return;
		}

		if(m_mi.is_single_value())
		{
			// TODO:
		}
		else
		{
			auto const mi_lower = m_mi.lower_bound();
			auto const mi_upper = m_mi.upper_bound();
			auto const range = mi_upper - mi_lower;

			for(auto const& itv : m_mi)
			{
				auto const l = (itv.lower() - mi_lower) / range;
				auto const u = (itv.upper() - mi_lower) / range;

				auto const x1 = std::min(std::floor(l * dev_width), dev_width - 1.0);
				auto const x2 = std::min(std::floor(u * dev_width), dev_width - 1.0);

				if(x1 == x2)
				{
					painter.drawLine(x1 + 0.5, 0.5, x2 + 0.5, dev_height - 0.5);
				}
				else
				{
					painter.drawRect(Wt::WRectF{ x1 + 0.5, 0.5, x2 - x1, dev_height - 1.0 });
				}
			}
		}
	}

private:
	multi_interval_t m_mi;
};


#endif


