// layout.h

#ifndef __WB_NN_VIS_LAYOUT_H
#define __WB_NN_VIS_LAYOUT_H

#include "../interface/nnet_fwd.h"

#include <map>


namespace nnet {

	namespace vis {

		struct rect
		{
			int left, top;
			int width, height;

			rect() {}
			rect(int l, int t, int w, int h): left(l), top(t), width(w), height(h) {}
		};

		struct point
		{
			int x, y;

			point() {}
			point(int _x, int _y): x(_x), y(_y) {}
		};

		struct path
		{
			typedef std::vector< point > subpath;

			std::vector< subpath > subpaths;
		};

		struct neuron_layout
		{
			rect rc_sum;
			rect rc_function;
			rect rc_activation;
			point pnt_input;
			point pnt_output;
			rect rc_neuron;
		};

		struct network_layout
		{
			size_t function_display_sz;
			std::map< neuron_id, neuron_layout > neurons;
		};


		struct DisplayOptions;


		std::map< ActivationFnType, path > generate_activation_fn_symbols(size_t sz);
		std::map< bool, path > generate_fn_axis_symbols(size_t sz);
		network_layout generate_layout(
			i_neuralnet const* net,
			rect& rc,
			double scale,
			double neuron_spacing,
			double layer_spacing,
			DisplayOptions display_options
			);

	}
}


#endif


