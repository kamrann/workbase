// layout.cpp

#include "layout.h"
#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"
#include "../interface/iterator.h"
#include "../interface/iterator_range.h"

#include "util/stl_util.hpp"

#include <algorithm>
#include <cmath>
#include <ratio>


namespace nnet {

	namespace vis {

		auto const MinimumScale = 10;
		auto const MaximumScale = 50;

		typedef std::ratio< 1, 1 > NeuronCoreSizeRatio;
		typedef std::ratio< 1, 1 > NeuronActivationSizeRatio;
		typedef std::ratio< 1, 5 > NeuronComponentGapRatio;

		typedef std::ratio_max< NeuronCoreSizeRatio, NeuronActivationSizeRatio > NeuronWidthRatio;

		// TODO: Treat as relative to scale, or relative to NeuronCoreSizeRatio?
		typedef std::ratio< 1, 2 > NeuronHGapRatio;
		typedef std::ratio< 1, 2 > NeuronVGapRatio;
		typedef std::ratio< 10, 1 > BiasGapRatio;
		typedef std::ratio< 10, 1 > InputHiddenGapRatio;
		typedef std::ratio< 10, 1 > HiddenOutputGapRatio;
		//


		std::map< ActivationFn, path > generate_activation_fn_symbols(size_t sz)
		{
			std::map< ActivationFn, path > mp;

			int const adj_sz = sz - 1;
			
			// Linear
			{
				path p;
				p.subpaths.emplace_back(path::subpath{ point{ 0, adj_sz }, point{ adj_sz, 0 } });
				mp[ActivationFn::Linear] = p;
			}

			// Bounded Linear
			{
				path p;
				p.subpaths.emplace_back(path::subpath{
					point{ 0, adj_sz * 5 / 6 },
					point{ adj_sz / 6, adj_sz * 5 / 6 },
					point{ adj_sz * 5 / 6, adj_sz / 6 },
					point{ adj_sz, adj_sz / 6 },
				});
				mp[ActivationFn::LinearBounded] = p;
			}

			// Thresholds
			{
				path p;
				p.subpaths.emplace_back(path::subpath{
					point{ 0, adj_sz },
					point{ adj_sz / 2, adj_sz },
					point{ adj_sz / 2, 0 },
					point{ adj_sz, 0 },
				});
				mp[ActivationFn::Threshold] = p;
				mp[ActivationFn::ThresholdSymmetric] = p;
			}

			// Sigmoids
			{
				auto segments = std::max(4u, sz / 2u);
				path::subpath s;
				for(size_t i = 0; i < segments + 1; ++i)
				{
					point pnt;
					auto const extreme = 8.0;
					auto x = -extreme + ((double)i / segments) * 2 * extreme;
					s.emplace_back(point{
						(int)(adj_sz * (x + extreme) / (2 * extreme)),
						(int)(adj_sz * (1.0 - (1.0 / (1.0 + std::exp(-x)))))
					});
				}
				path p;
				p.subpaths.emplace_back(s);
				mp[ActivationFn::Sigmoid] = p;
				mp[ActivationFn::SigmoidSymmetric] = p;
			}

			// Gaussians
			{
				auto segments = std::max(4u, sz / 2u);
				path::subpath s;
				for(size_t i = 0; i < segments + 1; ++i)
				{
					point pnt;
					auto const extreme = 3.0;
					auto x = -extreme + ((double)i / segments) * 2 * extreme;
					s.emplace_back(point{
						(int)(adj_sz * (x + extreme) / (2 * extreme)),
						(int)(adj_sz * (1.0 - std::exp(-x*x)))
					});
				}
				path p;
				p.subpaths.emplace_back(s);
				mp[ActivationFn::Gaussian] = p;
				mp[ActivationFn::GaussianSymmetric] = p;
			}

			// Sine
			{
				auto segments = std::max(4u, sz / 2u);
				path::subpath s;
				for(size_t i = 0; i < segments + 1; ++i)
				{
					point pnt;
					auto x = -M_PI + ((double)i / segments) * 2 * M_PI;
					s.emplace_back(point{
						(int)(adj_sz * (x + M_PI) / (2 * M_PI)),
						(int)(adj_sz * (1.0 - (std::sin(x) + 1.0) * 0.5))
					});
				}
				path p;
				p.subpaths.emplace_back(s);
				mp[ActivationFn::Sine] = p;
			}

			// Cosine
			{
				auto segments = std::max(4u, sz / 2u);
				path::subpath s;
				for(size_t i = 0; i < segments + 1; ++i)
				{
					point pnt;
					auto x = -3 * M_PI / 2 + ((double)i / segments) * 3 * M_PI;
					s.emplace_back(point{
						(int)(adj_sz * (x + 3 * M_PI / 2) / (3 * M_PI)),
						(int)(adj_sz * (1.0 - (std::cos(x) + 1.0) * 0.5))
					});
				}
				path p;
				p.subpaths.emplace_back(s);
				mp[ActivationFn::Cosine] = p;
			}

			return mp;
		}

		std::map< bool, path > generate_fn_axis_symbols(size_t sz)
		{
			int const adj_sz = sz - 1;

			path symmetric_p;
			symmetric_p.subpaths.emplace_back(path::subpath{ point{ 0, adj_sz / 2 }, point{ adj_sz, adj_sz / 2 } });
			symmetric_p.subpaths.emplace_back(path::subpath{ point{ adj_sz / 2, 0 }, point{ adj_sz / 2, adj_sz } });

			path asymmetric_p;
			asymmetric_p.subpaths.emplace_back(path::subpath{ point{ 0, adj_sz }, point{ adj_sz, adj_sz } });
			asymmetric_p.subpaths.emplace_back(path::subpath{ point{ adj_sz / 2, 0 }, point{ adj_sz / 2, adj_sz } });

			return{
					{ true, symmetric_p },
					{ false, asymmetric_p }
			};
		}

		// Excludes bias neurons
		size_t layout_standard_layer(
			neuron_range const& neurons,
			network_layout& layout,
			size_t x_pos,
			size_t y_pos,
			size_t max_across,
			size_t n_width,
			size_t n_height,
			size_t core_size,
			size_t act_size,
			size_t component_gap,
			size_t h_gap,
			size_t v_gap
			)
		{
			auto core_x_off = (n_width - core_size) / 2;
			auto act_x_off = (n_width - act_size) / 2;

			size_t x_index = 0;
			size_t y_index = 0;
			for(auto const& nr : neurons)
			{
				if(x_index == max_across)
				{
					x_index = 0;
					++y_index;
				}

				// Currently hard-coding for downwards layout (inputs at the top)
				auto const x_base = x_pos + x_index * (n_width + h_gap);
				auto y_base = y_pos + y_index * (n_height + v_gap);

				neuron_layout nr_layout;
				nr_layout.rc_neuron = rect(
					x_base,
					y_base,
					n_width,
					n_height
					);

				if(has_inputs(nr.type))
				{
					nr_layout.pnt_input = point(
						x_base + n_width / 2,
						y_base
						);
					nr_layout.rc_sum = rect(
						x_base + core_x_off,
						y_base,
						core_size,
						core_size
						);
					y_base += core_size + component_gap;
					nr_layout.rc_function = rect(
						x_base + core_x_off,
						y_base,
						core_size,
						core_size
						);
					y_base += core_size + component_gap;
				}

				nr_layout.rc_activation = rect(
					x_base + act_x_off,
					y_base,
					act_size,
					act_size
					);
				y_base += act_size;
				nr_layout.pnt_output = point(
					x_base + n_width / 2,
					y_base
					);
				
				layout.neurons.insert(std::make_pair(nr.id, nr_layout));

				++x_index;
			}

			return y_index * (n_height + v_gap) + n_height;
		}

		network_layout generate_layout(i_neuralnet const* net, rect& rc, double scale)
		{
			auto const width = rc.width;
			auto const pixel_scale = MinimumScale + (size_t)((MaximumScale - MinimumScale) * scale);

			auto const neuron_core_sz = std::ratio_product< NeuronCoreSizeRatio >(pixel_scale);
			auto const neuron_activation_sz = std::ratio_product< NeuronActivationSizeRatio >(pixel_scale);
			auto const component_gap = std::ratio_product< NeuronComponentGapRatio >(pixel_scale);
			auto const neuron_width = std::ratio_product< NeuronWidthRatio >(pixel_scale);
			auto const bias_gap = std::ratio_product< BiasGapRatio >(pixel_scale);
			auto const neuron_h_gap = std::ratio_product< NeuronHGapRatio >(pixel_scale);
			auto const neuron_v_gap = std::ratio_product< NeuronVGapRatio >(pixel_scale);
			auto const input_hidden_gap = std::ratio_product< InputHiddenGapRatio >(pixel_scale);
			auto const hidden_output_gap = std::ratio_product< HiddenOutputGapRatio >(pixel_scale);

			auto const non_bias_width = width - neuron_width - bias_gap;
			auto const max_neurons_across = (size_t)((non_bias_width + neuron_h_gap) / (neuron_width + neuron_h_gap));

			auto neuron_access = net->neuron_access();
			auto input_neurons = neuron_access->get_range(neuron_filter::Input);
			auto hidden_neurons = neuron_access->get_range(neuron_filter::Hidden);
			auto output_neurons = neuron_access->get_range(neuron_filter::Output);

			auto x_pos = rc.left;
			auto y_pos = rc.top;

			network_layout layout;
			layout.function_display_sz = neuron_activation_sz;

			// First, input layer
			auto n_height = neuron_activation_sz;
			auto input_height = layout_standard_layer(
				input_neurons,
				layout,
				x_pos,
				y_pos,
				max_neurons_across,
				neuron_width,
				n_height,
				neuron_core_sz,
				neuron_activation_sz,
				component_gap,
				neuron_h_gap,
				neuron_v_gap
				);
			y_pos += input_height;

			y_pos += input_hidden_gap;

			n_height = neuron_core_sz * 2 + neuron_activation_sz + component_gap * 2;

			// Hidden neurons
			auto hidden_height = layout_standard_layer(
				hidden_neurons,
				layout,
				x_pos,
				y_pos,
				max_neurons_across,
				neuron_width,
				n_height,
				neuron_core_sz,
				neuron_activation_sz,
				component_gap,
				neuron_h_gap,
				neuron_v_gap
				);
			y_pos += hidden_height;

			y_pos += hidden_output_gap;

			// Output neurons
			auto output_height = layout_standard_layer(
				output_neurons,
				layout,
				x_pos,
				y_pos,
				max_neurons_across,
				neuron_width,
				n_height,
				neuron_core_sz,
				neuron_activation_sz,
				component_gap,
				neuron_h_gap,
				neuron_v_gap
				);
			y_pos += output_height;

			rc.height = y_pos;
			return layout;
		}

	}
}




