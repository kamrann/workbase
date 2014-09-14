// coordinator.cpp

#include "coordinator.h"


namespace nnet {

	namespace vis {

		coordinator::coordinator()
		{
			m_display_mode = DisplayMode::Structure;
			m_zoom_level = 0.5;
			m_spacing_scale = 0.5;
		}

		void coordinator::set_network(std::shared_ptr< i_neuralnet > net)
		{
			m_net = std::move(net);
			on_changed(changed_event{ changed_event::Flags::Network });
		}

		i_neuralnet* coordinator::network()
		{
			return m_net.get();
		}

		void coordinator::register_changed_handler(settings_changed_handler_fn fn)
		{
			m_changed_handlers.emplace_back(std::move(fn));
		}
			
		void coordinator::register_feedback_handler(feedback_handler_fn fn)
		{
			m_feedback_handlers.emplace_back(std::move(fn));
		}

		void coordinator::on_changed(changed_event const& evt) const
		{
			for(auto const& handler : m_changed_handlers)
			{
				handler(evt);
			}
		}

		void coordinator::on_feedback(feedback_event const& evt) const
		{
			for(auto const& handler : m_feedback_handlers)
			{
				handler(evt);
			}
		}

		void coordinator::set_display_mode(DisplayMode mode)
		{
			m_display_mode = mode;
			on_changed(changed_event{ changed_event::Flags::DispMode });
		}

		void coordinator::set_zoom_level(double zm)
		{
			m_zoom_level = zm;
			on_changed(changed_event{ changed_event::Flags::Zoom });
		}

		void coordinator::set_spacing_scale(double s)
		{
			m_spacing_scale = s;
			on_changed(changed_event{ changed_event::Flags::SpacingScale });
		}

		DisplayMode coordinator::display_mode() const
		{
			return m_display_mode;
		}

		double coordinator::zoom_level() const
		{
			return m_zoom_level;
		}

		double coordinator::spacing_scale() const
		{
			return m_spacing_scale;
		}

		void coordinator::provide_feedback(feedback_event const& evt)
		{
			on_feedback(evt);
		}

	}
}


