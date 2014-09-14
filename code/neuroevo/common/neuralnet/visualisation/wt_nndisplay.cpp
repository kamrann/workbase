// wt_nndisplay.cpp

#include "wt_nndisplay.h"

#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"
#include "../interface/iterator.h"
#include "../interface/iterator_range.h"
#include "../interface/connection_access.h"
#include "../interface/internal_state_access.h"

#include <Wt/WPainter>
#include <Wt/WPaintDevice>
#include <Wt/WRectArea>


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

			void draw_net(i_neuralnet const* net, network_layout const& layout, Wt::WPainter& painter)
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
				painter.save();
				for(auto const& conn : *connection_access)
				{
					// TEMP
					if(layout.neurons.find(conn.src) == layout.neurons.end() ||
						layout.neurons.find(conn.dst) == layout.neurons.end())
					{
						continue;
					}
					//

					auto const signal = state.connections[std::make_pair(conn.src, conn.dst)].signal;
					Wt::WColor cl(0, 0, 0);
					if(signal > 0.0)
					{
						cl.setRgb(0, 255, 0);
					}
					else if(signal < 0.0)
					{
						cl.setRgb(255, 0, 0);
					}
					painter.setPen(Wt::WPen(cl));
					auto const& src_n = layout.neurons.at(conn.src);
					auto const& dst_n = layout.neurons.at(conn.dst);
					painter.drawLine(
						to_wt(src_n.pnt_output),
						to_wt(dst_n.pnt_input)
						);
				}
				painter.restore();

				auto neuron_access = net->neuron_access();
				auto all_neurons = neuron_access->get_range();
				painter.save();
				for(auto const& nr : all_neurons)
				{
					// TEMP
					if(layout.neurons.find(nr.id) == layout.neurons.end())
					{
						continue;
					}
					//

					if(has_inputs(nr.type))
					{
						auto const sum = state.neurons[nr.id].sum;
						Wt::WColor cl(255, 255, 255);
						if(sum > 0.0)
						{
							cl.setRgb(0, 255, 0);
						}
						else if(sum < 0.0)
						{
							cl.setRgb(255, 0, 0);
						}
						painter.setBrush(Wt::WBrush(cl));
						auto rc_sum = to_wt(layout.neurons.at(nr.id).rc_sum);
						painter.drawRect(rc_sum);

						// Draw function
						painter.save();
						auto rc_fn = to_wt(layout.neurons.at(nr.id).rc_function);
						painter.translate(rc_fn.left(), rc_fn.top());
						draw_function(
							axis_symbols[is_symmetric(nr.activation_fn)],
							act_fn_symbols[nr.activation_fn],
							painter);
						painter.restore();
					}

					auto const act = state.neurons[nr.id].activation;
					Wt::WColor cl(255, 255, 255);
					if(act > 0.0)
					{
						cl.setRgb(0, 255, 0);
					}
					else if(act < 0.0)
					{
						cl.setRgb(255, 0, 0);
					}
					painter.setBrush(Wt::WBrush(cl));
					auto rc_act = to_wt(layout.neurons.at(nr.id).rc_activation);
					painter.drawEllipse(rc_act);
				}
				painter.restore();
				
				painter.restore();
			}


			WNNFunctionWidget::WNNFunctionWidget(ActivationFn fn, size_t width)
			{
				// TODO: Overkill, need generate(ActFn, Size)
				auto act_fn_symbols = generate_activation_fn_symbols(width);
				auto axis_symbols = generate_fn_axis_symbols(width);

				m_axis_path = axis_symbols[is_symmetric(fn)];
				m_fn_path = act_fn_symbols[fn];

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
				auto handler = [this](changed_event const& evt)
				{
					if(evt.is_layout_affected())
					{
						update_layout();
					}
					update();
				};
				m_coordinator->register_changed_handler(handler);
			}

			void WNNDisplay::update_layout()
			{
				auto net = m_coordinator->network();

				rect rc_layout(0, 0, (int)width().toPixels(), (int)height().toPixels());
				m_layout = generate_layout(net, rc_layout, m_coordinator->zoom_level());
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
						m_coordinator->provide_feedback(
							feedback_event{ feedback_event::Type::HoverStart, id });
					}));
					area->mouseWentOut().connect(std::bind([this, id]
					{
						m_coordinator->provide_feedback(
							feedback_event{ feedback_event::Type::HoverEnd, id });
					}));

					addArea(area);
				}
			}

			void WNNDisplay::layoutSizeChanged(int width, int height)
			{
				update_layout();
				WPaintedWidget::layoutSizeChanged(width, height);
			}

			void WNNDisplay::paintEvent(Wt::WPaintDevice* device)
			{
				Wt::WPainter painter(device);

				auto net = m_coordinator->network();
				if(net)
				{
					draw_net(net, m_layout, painter);
				}
			}

		}
	}
}


