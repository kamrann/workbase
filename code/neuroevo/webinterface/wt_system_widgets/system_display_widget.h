// system_display_widget.h

#ifndef __SYSTEM_DISPLAY_WIDGET_H
#define __SYSTEM_DISPLAY_WIDGET_H

#include "../rtp_interface/systems/rtp_system_drawer.h"
#include "../rtp_interface/systems/rtp_interactive_input.h"

#include <Wt/WPaintedWidget>
#include <Wt/WPaintDevice>
#include <Wt/WPainter>

#include <mutex>


class sys_display_widget: public Wt::WPaintedWidget
{
public:
	enum {
		Margin = 10,
		LineWidth = 1,
	};

public:
	sys_display_widget(std::mutex& mtx): m_sys_drawer(), m_mtx(mtx), m_zoom(1.0)
	{
		setLayoutSizeAware(true);

		setAttributeValue("tabindex", "0");

		keyWentDown().connect([this](Wt::WKeyEvent e)
		{
			switch(e.key())
			{
				case Wt::Key_PageDown:
				m_zoom /= 1.5;
				return;
				case Wt::Key_PageUp:
				m_zoom *= 1.5;
				return;
			}

			auto it = m_input_states.find(e.key());
			if(it != m_input_states.end())
			{
				bool& active = it->second;
				if(!active)
				{
					m_input_signal.emit(rtp::interactive_input(e.key(), true));
					active = true;
				}
			}
		}
		);

		keyWentUp().connect([this](Wt::WKeyEvent e)
		{
			auto it = m_input_states.find(e.key());
			if(it != m_input_states.end())
			{
				bool& active = it->second;
				if(active)
				{
					m_input_signal.emit(rtp::interactive_input(e.key(), false));
					active = false;
				}
				else
				{
					throw std::exception("keyWentUp(): Input not active!");
				}
			}
		}
		);
	}

public:
	void set_drawer(std::unique_ptr< rtp::i_system_drawer > drawer)
	{
		m_sys_drawer = std::move(drawer);
	}

	rtp::i_system_drawer* get_drawer()
	{
		return m_sys_drawer.get();
	}

	void enable_interaction(rtp::interactive_input_set const& required)
	{
		//m_required_inputs = required;
		m_input_states.clear();
		std::transform(
			std::begin(required),
			std::end(required),
			std::inserter(m_input_states, m_input_states.end()),
			[](unsigned long key)
		{
			return std::make_pair(key, false);
		});
	}

	Wt::Signal< rtp::interactive_input >& interactive_input_sig()
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

			std::lock_guard< std::mutex > guard(m_mtx);
			rtp::i_system_drawer::options_t options;
			options.zoom = m_zoom;
			m_sys_drawer->draw_system(painter, options);
		}
	}

private:
	std::unique_ptr< rtp::i_system_drawer > m_sys_drawer;
	//rtp::interactive_input_set m_required_inputs;
	std::map< unsigned long, bool > m_input_states;
	Wt::Signal< rtp::interactive_input > m_input_signal;
	double m_zoom;
	std::mutex& m_mtx;
};


#endif


