// wt_nndisplay.cpp

#include "wt_nndisplay.h"

#include "../interface/neuralnet.h"
#include "../interface/activation_functions.h"
#include "../interface/neuron_access.h"
#include "../interface/iterator.h"
#include "../interface/iterator_range.h"
#include "../interface/connection_access.h"
#include "../interface/internal_state_access.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>
#include <Wt/WRectArea>

#include <boost/optional.hpp>


namespace nnet {
	namespace vis {
		namespace wt {

			inline Wt::WRectF to_wt(rect const& rc)
			{
				return Wt::WRectF(
					rc.left + 0.5,
					rc.top + 0.5,
					rc.width - 1.0,
					rc.height - 1.0
					);
			}

			inline Wt::WPointF to_wt(point const& pnt)
			{
				return Wt::WPointF(
					pnt.x + 0.5,
					pnt.y + 0.5
					);
			}
			/*
			Wt::WRectF calculate_display_size(i_neuralnet const* net, size_t available_width)
			{
				rect rc{ 0, 0, (int)available_width, 0 };
				generate_layout(net, rc);
				return Wt::WRectF(
					0.0,
					0.0,
					rc.width,
					rc.height
					);
			}
			*/

			void draw_path(path const& pth, Wt::WPainter& painter)
			{
				for(auto const& sub : pth.subpaths)
				{
					Wt::WPainterPath wt_path;
					wt_path.moveTo(to_wt(sub[0]));
					for(size_t i = 1; i < sub.size(); ++i)
					{
						wt_path.lineTo(to_wt(sub[i]));
					}
					painter.drawPath(wt_path);
				}
			}

			void draw_function(path const& axis_path, path const& fn_path, Wt::WPainter& painter)
			{
				painter.save();
				painter.setBrush(Wt::WBrush(Wt::NoBrush));
				Wt::WPen pen;
				pen.setColor(Wt::WColor(180, 180, 180));
				painter.setPen(pen);
				draw_path(axis_path, painter);

				pen.setColor(Wt::WColor(0, 0, 0));
				painter.setPen(pen);
				draw_path(fn_path, painter);
				painter.restore();
			}

			Wt::WColor get_input_sum_color(
				DisplayMode mode,
				activity_state const& state,
				neuron_data const& nr
				)
			{
				Wt::WColor cl{ Wt::white };

				switch(mode)
				{
					case DisplayMode::State:
					{
						auto const sum = state.neurons.at(nr.id).sum;
						if(sum > 0.0)
						{
							cl.setRgb(0, 255, 0);
						}
						else if(sum < 0.0)
						{
							cl.setRgb(255, 0, 0);
						}
					}
					break;
				}

				return cl;
			}

			Wt::WColor get_activation_color(
				DisplayMode mode,
				activity_state const& state,
				neuron_data const& nr
				)
			{
				Wt::WColor cl{ Wt::white };

				switch(mode)
				{
					case DisplayMode::State:
					{
						auto const act = state.neurons.at(nr.id).activation;
						auto const strength = std::min(std::abs(act), 1.0);
						auto const pos_strength = std::max(act, 0.0);
						auto const neg_strength = -std::min(act, 0.0);
						cl.setRgb(255 * (1.0 - pos_strength), 255 * (1.0 - neg_strength), 255 * (1.0 - strength));
					}
					break;
				}

				return cl;
			}

			struct connection_gradient
			{
				Wt::WColor src_cl, dst_cl;
			};

			connection_gradient get_connection_gradient(
				DisplayMode mode,
				DisplayOptions options,
				activity_state const& state,
				connection_data const& cn
				)
			{
				connection_gradient grad{
					Wt::black,
					Wt::black
				};

				switch(mode)
				{
					case DisplayMode::Structure:
					{
						if(options.test(DisplayFlags::ColourWeights))
						{
							auto const weight = cn.weight;
							if(weight > 0.0)
							{
								grad.src_cl = grad.dst_cl = Wt::green;
							}
							else
							{
								grad.src_cl = grad.dst_cl = Wt::red;
							}
						}
					}
					break;

					case DisplayMode::State:
					{
						auto const signal = state.connections.at(std::make_pair(cn.src, cn.dst)).signal;
						if(signal > 0.0)
						{
							grad.dst_cl.setRgb(0, 255, 0);
							grad.src_cl = grad.dst_cl;
						}
						else if(signal < 0.0)
						{
							grad.dst_cl.setRgb(255, 0, 0);
							grad.src_cl = grad.dst_cl;
						}

						if(options.test(DisplayFlags::SignalGradients))
						{
							auto const weight = cn.weight;
							if(weight < 0.0)
							{
								grad.src_cl.setRgb(
									255 - grad.dst_cl.red(),
									255 - grad.dst_cl.green(),
									0
									);
							}
						}
					}
					break;
				}

				return grad;
			}

			void draw_net(
				i_neuralnet const* net,
				network_layout const& layout,
				boost::optional< neuron_id > highlighted,
				DisplayMode mode,
				DisplayOptions display_options,
				Wt::WPainter& painter)
			{
				auto device = painter.device();
				auto pix_width = device->width().toPixels();
				auto pix_height = device->height().toPixels();

				rect rc_layout(0, 0, (int)pix_width, (int)pix_height);
				auto act_fn_symbols = generate_activation_fn_symbols(layout.function_display_sz);
				auto axis_symbols = generate_fn_axis_symbols(layout.function_display_sz);

				auto state = net->current_activity_state();

				painter.save();

				Wt::WPen pen(Wt::black);
				painter.setPen(pen);

				auto connection_access = net->connection_access();
				auto all_connections = connection_access->get_range();
				// TODO: inefficient
				if(display_options.test(DisplayFlags::Connections) || highlighted)
				{
					painter.save();
					for(auto const& conn : all_connections)
					{
						if(highlighted &&
							conn.src != *highlighted &&
							conn.dst != *highlighted
							)
						{
							continue;
						}

						auto const& src_n = layout.neurons.at(conn.src);
						auto const& dst_n = layout.neurons.at(conn.dst);

						auto src_pnt = to_wt(src_n.pnt_output);
						auto dst_pnt = to_wt(dst_n.pnt_input);
							
						auto gradient = get_connection_gradient(mode, display_options, state, conn);
						Wt::WGradient wt_gradient;
						wt_gradient.setLinearGradient(
							src_pnt.x(),
							src_pnt.y(),
							dst_pnt.x(),
							dst_pnt.y()
							);
						wt_gradient.addColorStop(0.0, gradient.src_cl);
						wt_gradient.addColorStop(1.0, gradient.dst_cl);
						Wt::WPen pen;
						//pen.setGradient(wt_gradient);
						// TODO: Waiting on Wt gradient bug
						pen.setColor(gradient.dst_cl);
						//
						painter.setPen(pen);
						painter.drawLine(
							src_pnt,
							dst_pnt
							);
					}
					painter.restore();
				}

				auto neuron_access = net->neuron_access();
				auto all_neurons = neuron_access->get_range();
				painter.save();
				for(auto const& nr : all_neurons)
				{
					if(has_inputs(nr.type))
					{
						// Draw input sum
						auto cl = get_input_sum_color(mode, state, nr);
						painter.setBrush(Wt::WBrush(cl));
						auto rc_sum = to_wt(layout.neurons.at(nr.id).rc_sum);
						painter.drawRect(rc_sum);

						if(display_options.test(DisplayFlags::ActivationFunction))
						{
							// Draw function
							painter.save();
							auto rc_fn = to_wt(layout.neurons.at(nr.id).rc_function);
							painter.translate(rc_fn.left(), rc_fn.top());
							draw_function(
								axis_symbols[is_symmetric(nr.activation_fn.type)],
								act_fn_symbols[nr.activation_fn.type],
								painter);
							painter.restore();
						}
					}

					// Draw activation
					auto cl = get_activation_color(mode, state, nr);
					painter.setBrush(Wt::WBrush(cl));
					auto rc_act = to_wt(layout.neurons.at(nr.id).rc_activation);
					painter.drawEllipse(rc_act);

					if(highlighted && *highlighted == nr.id)
					{
						painter.save();
						Wt::WPen pen(Wt::blue);
						painter.setPen(pen);
						painter.setBrush(Wt::NoBrush);
						auto rc_neuron = to_wt(layout.neurons.at(nr.id).rc_neuron);
						painter.drawRect(rc_neuron);
						painter.restore();
					}
				}
				painter.restore();
				
				painter.restore();
			}


			WNNFunctionWidget::WNNFunctionWidget(activation_function fn, size_t width)
			{
				// TODO: Overkill, need generate(ActFn, Size)
				auto act_fn_symbols = generate_activation_fn_symbols(width);
				auto axis_symbols = generate_fn_axis_symbols(width);

				m_axis_path = axis_symbols[is_symmetric(fn.type)];
				m_fn_path = act_fn_symbols[fn.type];

				resize(width, width);
			}

			void WNNFunctionWidget::paintEvent(Wt::WPaintDevice* device)
			{
				Wt::WPainter painter(device);

				draw_function(m_axis_path, m_fn_path, painter);
			}


			WNNDisplay::WNNDisplay(std::shared_ptr< coordinator > crd):
				m_coordinator(std::move(crd))
			{
				setStyleClass("nn_display");

				auto handler = [this](changed_event const& evt)
				{
					if(evt.is_layout_affected())
					{
						update_layout(0, 0);	// TODO: For some reason, widget width()/height() returning zero anyway
					}
					update();
				};
				m_coordinator->register_changed_handler(handler);
			}

			void WNNDisplay::update_layout(int width, int height)
			{
				auto net = m_coordinator->network();

				rect rc_layout(0, 0, width, height);
				m_layout = generate_layout(
					net,
					rc_layout,
					m_coordinator->neuron_scale(),
					m_coordinator->neuron_spacing(),
					m_coordinator->layer_spacing(),
					m_coordinator->display_options()
					);
				setMinimumSize(Wt::WLength::Auto, rc_layout.height);
				setMaximumSize(Wt::WLength::Auto, rc_layout.height);
				//resize(100/*Wt::WLength::Auto*/, rc_layout.height);

				auto existing = areas();
				for(auto a : existing)
				{
					removeArea(a);
					delete a;
				}

				for(auto const& entry : m_layout.neurons)
				{
					auto const& id = entry.first;
					auto const& nr = entry.second;
					
					auto area = new Wt::WRectArea(
						nr.rc_neuron.left,
						nr.rc_neuron.top,
						nr.rc_neuron.width,
						nr.rc_neuron.height
						);

					area->mouseWentOver().connect(std::bind([this, id]
					{
						m_coordinator->set_hover_neuron(id);

						// TODO: Think feedback events are unnecessary, better to just incorporate everything into
						// set/get and changes mechanism.
						m_coordinator->provide_feedback(
							feedback_event{ feedback_event::Type::HoverStart, id });
					}));
					area->mouseWentOut().connect(std::bind([this, id]
					{
						m_coordinator->set_hover_neuron(boost::none);

						m_coordinator->provide_feedback(
							feedback_event{ feedback_event::Type::HoverEnd, id });
					}));
					area->clicked().connect(std::bind([this, id]
					{
						m_coordinator->set_selected_neuron(id);

//						m_coordinator->provide_feedback(
//							feedback_event{ feedback_event::Type::HoverEnd, id });
					}));

					addArea(area);
				}

				auto bg_area = new Wt::WRectArea(Wt::WRectF(0, 0, width, height));
				bg_area->clicked().connect(std::bind([this]
				{
					m_coordinator->set_selected_neuron(boost::none);

					//						m_coordinator->provide_feedback(
					//							feedback_event{ feedback_event::Type::HoverEnd, id });
				}));
				addArea(bg_area);
			}

			void WNNDisplay::layoutSizeChanged(int width, int height)
			{
				update_layout(width, height);
				WPaintedWidget::layoutSizeChanged(width, height);
			}

			void WNNDisplay::paintEvent(Wt::WPaintDevice* device)
			{
				Wt::WPainter painter(device);

				auto net = m_coordinator->network();
				if(net)
				{
					auto highlighted = m_coordinator->current_neuron();
					draw_net(
						net,
						m_layout,
						highlighted,
						m_coordinator->display_mode(),
						m_coordinator->display_options(),
						painter
						);
				}
			}

		}
	}
}


