// system_display_widget.h

#ifndef __SYSTEM_DISPLAY_WIDGET_H
#define __SYSTEM_DISPLAY_WIDGET_H

#include "../rtp_interface/systems/rtp_system_drawer.h"
#include "../rtp_interface/systems/rtp_interactive_input.h"

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>


class sys_display_widget: public Wt::WPaintedWidget
{
public:
	enum {
		Margin = 10,
		LineWidth = 1,
	};

public:
	sys_display_widget(boost::mutex& mtx): m_sys_drawer(nullptr), m_mtx(mtx)
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
				case Wt::Key_Q:		idx = 0;	break;
				case Wt::Key_W:		idx = 1;	break;
				case Wt::Key_E:		idx = 2;	break;
				case Wt::Key_R:		idx = 3;	break;
				case Wt::Key_T:		idx = 4;	break;
				case Wt::Key_Y:		idx = 5;	break;
				default: return;
			}
			m_input_signal.emit(interactive_input(idx, true));
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
				case Wt::Key_Q:		idx = 0;	break;
				case Wt::Key_W:		idx = 1;	break;
				case Wt::Key_E:		idx = 2;	break;
				case Wt::Key_R:		idx = 3;	break;
				case Wt::Key_T:		idx = 4;	break;
				case Wt::Key_Y:		idx = 5;	break;
				default: return;
			}
			m_input_signal.emit(interactive_input(idx, false));
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

	Wt::Signal< interactive_input >& interactive_input_sig()
	{
		return m_input_signal;
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
			// TODO: Not currently threadsafe with asio implementation of system updating!!!!!!!!!!!!!!!!
			Wt::WPainter painter(device);

			boost::lock_guard< boost::mutex > guard(m_mtx);
			m_sys_drawer->draw_system(painter);
		}
	}

private:
	i_system_drawer* m_sys_drawer;
	bool m_interaction_enabled;
	Wt::Signal< interactive_input > m_input_signal;
	boost::mutex& m_mtx;
};


#endif


