// coordinator.cpp

#include "coordinator.h"


namespace nnet {

	namespace vis {

		coordinator::coordinator()
		{
			m_display_mode = DisplayMode::Structure;
			m_neuron_scale = 0.5;
			m_neuron_spacing = 0.5;
			m_layer_spacing = 0.5;
			m_display_options = DisplayOptions::Default;
			m_hover_neuron = boost::none;
			m_selected_neuron = boost::none;
		}

		void coordinator::set_network(std::shared_ptr< i_neuralnet > net)
		{
			m_net = std::move(net);
			// TODO: m_neuron_ranges = calculate_input_output_ranges(m_net.get());
			m_display_mode = DisplayMode::Structure;
			m_hover_neuron = boost::none;
			m_selected_neuron = boost::none;

			on_changed(changed_event{
				changed_event::Flags::Network |
				changed_event::Flags::DispMode |
				changed_event::Flags::HoverNeuron |
				changed_event::Flags::SelectedNeuron
			});
		}

		i_neuralnet* coordinator::network()
		{
			return m_net.get();
		}

		input_output_ranges const& coordinator::neuron_io_range(neuron_id id) const
		{
			return m_neuron_ranges.at(id);
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

		void coordinator::set_neuron_scale(double s)
		{
			m_neuron_scale = s;
			on_changed(changed_event{ changed_event::Flags::NeuronScale });
		}

		void coordinator::set_neuron_spacing(double s)
		{
			m_neuron_spacing = s;
			on_changed(changed_event{ changed_event::Flags::NeuronSpacing });
		}

		void coordinator::set_layer_spacing(double s)
		{
			m_layer_spacing = s;
			on_changed(changed_event{ changed_event::Flags::LayerSpacing });
		}

		void coordinator::set_display_options(uint32_t flags)
		{
			m_display_options = flags;
			on_changed(changed_event{ changed_event::Flags::DisplayOption });
		}
			
		void coordinator::toggle_display_option(uint32_t flag, bool on)
		{
			m_display_options.toggle(flag, on);
			on_changed(changed_event{ changed_event::Flags::DisplayOption });
		}

		void coordinator::set_hover_neuron(boost::optional< neuron_id > id)
		{
			m_hover_neuron = id;
			on_changed(changed_event{ changed_event::Flags::HoverNeuron });
		}

		void coordinator::set_selected_neuron(boost::optional< neuron_id > id)
		{
			m_selected_neuron = id;
			on_changed(changed_event{ changed_event::Flags::SelectedNeuron });
		}

		DisplayMode coordinator::display_mode() const
		{
			return m_display_mode;
		}

		double coordinator::neuron_scale() const
		{
			return m_neuron_scale;
		}

		double coordinator::neuron_spacing() const
		{
			return m_neuron_spacing;
		}

		double coordinator::layer_spacing() const
		{
			return m_layer_spacing;
		}

		DisplayOptions coordinator::display_options() const
		{
			return m_display_options;
		}

		bool coordinator::has_display_option(uint32_t flag) const
		{
			return m_display_options.test(flag);
		}

		boost::optional< neuron_id > coordinator::hover_neuron() const
		{
			return m_hover_neuron;
		}

		boost::optional< neuron_id > coordinator::selected_neuron() const
		{
			return m_selected_neuron;
		}

		boost::optional< neuron_id > coordinator::current_neuron() const
		{
			return m_hover_neuron ? m_hover_neuron : m_selected_neuron;
		}

		void coordinator::provide_feedback(feedback_event const& evt)
		{
			on_feedback(evt);
		}

	}
}


