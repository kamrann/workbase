// wt_nndisplay.h

#ifndef __WB_NN_WT_DISPLAY_H
#define __WB_NN_WT_DISPLAY_H

#include "coordinator.h"
#include "layout.h"

#include <Wt/WPaintedWidget>

#include <memory>


namespace Wt {
	class WRectF;
	class WPainter;
	class WPaintDevice;
}

namespace nnet {
	namespace vis {

		class coordinator;

		namespace wt {

			//Wt::WRectF calculate_display_size(i_neuralnet const* net, size_t available_width);
			void draw_path(path const& pth, Wt::WPainter& painter);
			void draw_function(path const& axis_path, path const& fn_path, Wt::WPainter& painter);
			void draw_net(i_neuralnet const* net, Wt::WPainter& painter);

			class WNNFunctionWidget:
				public Wt::WPaintedWidget
			{
			public:
				WNNFunctionWidget(ActivationFn fn, size_t width);

			protected:
				void paintEvent(Wt::WPaintDevice* device);

			private:
				path m_axis_path;
				path m_fn_path;
			};

			class WNNDisplay:
				public Wt::WPaintedWidget
			{
			public:
				WNNDisplay(std::shared_ptr< coordinator > crd);

			protected:
				void update_layout();

			protected:
				void layoutSizeChanged(int width, int height);
				void paintEvent(Wt::WPaintDevice* device);

			protected:
				std::shared_ptr< coordinator > m_coordinator;
				network_layout m_layout;
			};

		}
	}
}


#endif


