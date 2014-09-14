// coordinator.h

#ifndef __WB_NN_VIS_COORDINATOR_H
#define __WB_NN_VIS_COORDINATOR_H

#include "../interface/nnet_fwd.h"

#include <memory>
#include <functional>
#include <list>


namespace nnet {

	namespace vis {

		enum class DisplayMode {
			Structure,
			// Edit?
			State,
		};

		struct changed_event
		{
			enum Flags {
				Network = 1 << 0,
				DispMode = 1 << 1,
				Zoom = 1 << 2,
				SpacingScale = 1 << 3,
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
				return test(Network | Zoom | SpacingScale);
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

			void register_changed_handler(settings_changed_handler_fn fn);
			void register_feedback_handler(feedback_handler_fn fn);

			void set_display_mode(DisplayMode mode);
			void set_zoom_level(double zm);
			void set_spacing_scale(double s);

			DisplayMode display_mode() const;
			double zoom_level() const;
			double spacing_scale() const;

			void provide_feedback(feedback_event const& evt);

		protected:
			void on_changed(changed_event const& evt) const;
			void on_feedback(feedback_event const& evt) const;

		protected:
			std::shared_ptr< i_neuralnet > m_net;
			std::list< settings_changed_handler_fn > m_changed_handlers;
			std::list< feedback_handler_fn > m_feedback_handlers;

			DisplayMode m_display_mode;
			double m_zoom_level;
			double m_spacing_scale;
		};

	}
}


#endif


