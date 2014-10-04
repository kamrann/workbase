// coordinator.h

#ifndef __WB_NN_VIS_COORDINATOR_H
#define __WB_NN_VIS_COORDINATOR_H

#include "display_options.h"
#include "../interface/nnet_fwd.h"
#include "../interface/algorithm.h"

#include <boost/optional.hpp>

#include <memory>
#include <functional>
#include <list>


namespace nnet {

	namespace vis {

		struct changed_event
		{
			enum Flags {
				Network = 1 << 0,
				DispMode = 1 << 1,
				NeuronScale = 1 << 2,
				NeuronSpacing = 1 << 3,
				LayerSpacing = 1 << 4,
				DisplayOption = 1 << 5,	// TODO: if want to differentiate, probably need subflags member
				HoverNeuron = 1 << 6,
				SelectedNeuron = 1 << 7,
			};

			uint32_t flags;

			inline bool test(uint32_t f) const
			{
				return (flags & f) != 0;
			}

			inline bool test_all(uint32_t f) const
			{
				return (flags & f) == f;
			}

			bool is_layout_affected() const
			{
				return test(Network | NeuronScale | NeuronSpacing | LayerSpacing | DisplayOption);
			}
		};

		struct feedback_event
		{
			enum class Type {
				HoverStart,
				HoverEnd,
				LeftClick,
				RightClick,
				DoubleClick,
			};

			Type type;
			neuron_id id;
		};

		class coordinator
		{
		public:
			coordinator();

		public:
			typedef std::function< void(changed_event const&) > settings_changed_handler_fn;
			typedef std::function< void(feedback_event const&) > feedback_handler_fn;

		public:
			void set_network(std::shared_ptr< i_neuralnet > net);
			i_neuralnet* network();
			input_output_ranges const& neuron_io_range(neuron_id id) const;

			void register_changed_handler(settings_changed_handler_fn fn);
			void register_feedback_handler(feedback_handler_fn fn);

			void set_display_mode(DisplayMode mode);
			void set_neuron_scale(double s);
			void set_neuron_spacing(double s);
			void set_layer_spacing(double s);
			void set_display_options(uint32_t flags);
			void toggle_display_option(uint32_t flag, bool on);

			void set_hover_neuron(boost::optional< neuron_id > id);
			void set_selected_neuron(boost::optional< neuron_id > id);

			DisplayMode display_mode() const;
			double neuron_scale() const;
			double neuron_spacing() const;
			double layer_spacing() const;
			DisplayOptions display_options() const;
			bool has_display_option(uint32_t flag) const;

			boost::optional< neuron_id > hover_neuron() const;
			boost::optional< neuron_id > selected_neuron() const;
			boost::optional< neuron_id > current_neuron() const;

			void provide_feedback(feedback_event const& evt);

		protected:
			void on_changed(changed_event const& evt) const;
			void on_feedback(feedback_event const& evt) const;

		protected:
			std::shared_ptr< i_neuralnet > m_net;
			input_output_range_map m_neuron_ranges;

			std::list< settings_changed_handler_fn > m_changed_handlers;
			std::list< feedback_handler_fn > m_feedback_handlers;

			DisplayMode m_display_mode;
			double m_neuron_scale;
			double m_neuron_spacing;
			double m_layer_spacing;
			DisplayOptions m_display_options;
			boost::optional< neuron_id > m_hover_neuron;
			boost::optional< neuron_id > m_selected_neuron;
		};

	}
}


#endif


