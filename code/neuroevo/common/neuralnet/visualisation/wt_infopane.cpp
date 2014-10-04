// wt_infopane.cpp

#include "wt_infopane.h"
#include "wt_nndisplay.h"
#include "text_output.h"
#include "coordinator.h"
#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"
#include "../interface/internal_state_access.h"

#include "util/interval_io.h"

#include <Wt/WVBoxLayout>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WCSSDecorationStyle>

#include <sstream>
#include <iomanip>


namespace nnet {
	namespace vis {
		namespace wt {

			const size_t PanelWidth = 200;

			WNNInfoPane::WNNInfoPane(std::shared_ptr< coordinator > crd):
				m_coordinator(std::move(crd))
			{
				auto vlayout = new Wt::WVBoxLayout();
				vlayout->setContentsMargins(0, 0, 0, 0);
				setLayout(vlayout);
				setMinimumSize(PanelWidth, Wt::WLength::Auto);

				m_info_table = new Wt::WTable;
				m_info_table->addStyleClass("outerfeedbacktable");
				vlayout->addWidget(m_info_table, 0, Wt::AlignTop);

/*				auto handler = [this](feedback_event const& evt)
				{
					process_feedback(evt);
				};
				m_coordinator->register_feedback_handler(handler);
*/
				auto handler = [this](changed_event const& evt)
				{
					on_status_changed(evt);
				};
				m_coordinator->register_changed_handler(handler);
			}

/*			void WNNInfoPane::process_feedback(feedback_event const& evt)
			{
				switch(evt.type)
				{
					case feedback_event::Type::HoverStart:
					on_hover_start(evt);
					break;
					case feedback_event::Type::HoverEnd:
					on_hover_end(evt);
					break;
				}
				auto net = m_coordinator->network();
			}
*/
			void WNNInfoPane::add_network_summary(Wt::WTable* table)
			{
				auto net = m_coordinator->network();

				std::stringstream ss;
				std::fixed(ss);
				ss.precision(3);

				auto row_idx = table->rowCount();

				auto row = table->rowAt(row_idx++);
				auto label = new Wt::WText("Network Info");
				label->setStyleClass("feedbackheader");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				row->elementAt(0)->decorationStyle().setBorder(
					Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

				row = table->rowAt(row_idx++);
				std::string title = "MLP";	// TODO:
				row->elementAt(0)->addWidget(new Wt::WText(title));
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				ss.str("");
				ss << net->input_count() << " Input" << (net->input_count() > 1 ? "s" : "");
				label = new Wt::WText(ss.str());
				label->addStyleClass("fb_subelement");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				ss.str("");
				ss << net->hidden_count() << " Hidden Neuron" << (net->hidden_count() > 1 ? "s" : "");
				label = new Wt::WText(ss.str());
				label->addStyleClass("fb_subelement");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				ss.str("");
				ss << net->output_count() << " Output" << (net->output_count() > 1 ? "s" : "");
				label = new Wt::WText(ss.str());
				label->addStyleClass("fb_subelement");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				row->elementAt(0)->decorationStyle().setBorder(
					Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

				if(m_coordinator->display_mode() == DisplayMode::State)
				{
					auto neuron_access = net->neuron_access();
					auto state = net->current_activity_state();

					row = table->rowAt(row_idx++);
					label = new Wt::WText("Inputs");
					label->setStyleClass("feedbackheader");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					auto inputs_table = new Wt::WTable();
					row->elementAt(0)->addWidget(inputs_table);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					inputs_table->addStyleClass("feedbacktable");

					auto inputs = neuron_access->get_range(layer_filter::Input, neuron_filter::Input);
					auto input_row_idx = 0u;
					for(auto const& ip: inputs)
					{
						row = inputs_table->rowAt(input_row_idx);
						
						label = new Wt::WText(std::to_string(input_row_idx + 1));
						label->addStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->decorationStyle().setBorder(
							Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Right);

						ss.str("");
						ss << state.neurons.at(ip.id).activation;
						label = new Wt::WText(ss.str());
						label->addStyleClass("fb_subelement");
						row->elementAt(1)->addWidget(label);
						row->elementAt(1)->setContentAlignment(Wt::AlignRight);

						++input_row_idx;
					}

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					row = table->rowAt(row_idx++);
					label = new Wt::WText("Outputs");
					label->setStyleClass("feedbackheader");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					auto outputs_table = new Wt::WTable();
					row->elementAt(0)->addWidget(outputs_table);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					outputs_table->addStyleClass("feedbacktable");

					auto outputs = neuron_access->get_range(layer_filter::Output, neuron_filter::Output);
					auto output_row_idx = 0u;
					for(auto const& op : outputs)
					{
						row = outputs_table->rowAt(output_row_idx);

						label = new Wt::WText(std::to_string(output_row_idx + 1));
						label->addStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->decorationStyle().setBorder(
							Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Right);

						ss.str("");
						ss << state.neurons.at(op.id).activation;
						label = new Wt::WText(ss.str());
						label->addStyleClass("fb_subelement");
						row->elementAt(1)->addWidget(label);
						row->elementAt(1)->setContentAlignment(Wt::AlignRight);

						++output_row_idx;
					}

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);
				}
			}

			void WNNInfoPane::add_neuron_summary(neuron_id id, Wt::WTable* table)
			{
				auto net = m_coordinator->network();
				auto nr_access = net->neuron_access();
				auto nr = nr_access->get_neuron_data(id);
				auto incoming = nr_access->get_incoming_connections(id);
				auto state = net->current_activity_state();

				std::stringstream ss;
				std::fixed(ss);

				auto row_idx = table->rowCount();

				auto row = table->rowAt(row_idx++);
				auto label = new Wt::WText("Neuron Info");
				label->setStyleClass("feedbackheader");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				row->elementAt(0)->decorationStyle().setBorder(
					Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

				row = table->rowAt(row_idx++);
				ss.str("");
				ss << neuron_name(net, id);
				label = new Wt::WText(ss.str());
				label->addStyleClass("fb_subelement");
				row->elementAt(0)->addWidget(label);
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				row->elementAt(0)->decorationStyle().setBorder(
					Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

				if(has_inputs(nr.type))
				{
					// Incoming
					row = table->rowAt(row_idx++);

					ss.str("");
					switch(m_coordinator->display_mode())
					{
						case DisplayMode::State:
						ss << "Incoming Signals";
						break;

						default:
						ss << "Incoming Connections";
						break;
					}
					label = new Wt::WText(ss.str());
					label->addStyleClass("feedbackheader");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					row = table->rowAt(row_idx++);
					auto incoming_table = new Wt::WTable();
					row->elementAt(0)->addWidget(incoming_table);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					incoming_table->addStyleClass("feedbacktable");
					size_t incoming_row_idx = 0;
					auto inputs = nr_access->get_incoming_connections(id);
					for(auto const& conn : inputs)
					{
						row = incoming_table->rowAt(incoming_row_idx++);
						auto col = 0u;

						auto label = new Wt::WText(neuron_name(net, conn.src, NeuronNameStyle::Short));
						label->addStyleClass("fb_subelement");
						row->elementAt(col)->addWidget(label);
						row->elementAt(col)->decorationStyle().setBorder(
							Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Right);
						++col;
						
						if(m_coordinator->display_mode() == DisplayMode::Structure)
						{
							ss.str("");
							ss << std::setprecision(3) << conn.weight;
							label = new Wt::WText(ss.str());
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignRight);
							++col;
						}

						if(m_coordinator->display_mode() == DisplayMode::State)
						{
							ss.str("");
							ss << std::setprecision(2) << state.neurons.at(conn.src).activation;
							label = new Wt::WText(ss.str());
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignRight);
							++col;

							label = new Wt::WText("*");
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignLeft);
							++col;

							ss.str("");
							ss << std::setprecision(2) << conn.weight;
							label = new Wt::WText(ss.str());
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignRight);
							++col;

							label = new Wt::WText("=");
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignLeft);
							++col;

							ss.str("");
							ss << std::setprecision(3) << state.connections.at(std::make_pair(conn.src, conn.dst)).signal;
							label = new Wt::WText(ss.str());
							label->addStyleClass("fb_subelement");
							row->elementAt(col)->addWidget(label);
							row->elementAt(col)->setContentAlignment(Wt::AlignRight);
							++col;
						}
					}

					if(m_coordinator->display_mode() == DisplayMode::Structure)
					{
						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Input Range " << m_coordinator->neuron_io_range(nr.id).input;// .to_string(2);
						label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
						label->addStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					}

					if(m_coordinator->display_mode() == DisplayMode::State)
					{
						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Sum = " << std::setprecision(3) << state.neurons.at(nr.id).sum;
						label = new Wt::WText(ss.str());
						label->addStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					}

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					// Activation
					row = table->rowAt(row_idx++);
					label = new Wt::WText("Activation");
					label->addStyleClass("feedbackheader");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					row->elementAt(0)->decorationStyle().setBorder(
						Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

					row = table->rowAt(row_idx++);
					ss.str("");
					ss << ActivationFnNames[(int)nr.activation_fn.type];
					label = new Wt::WText(ss.str());
					label->setStyleClass("fb_subelement");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					auto fn_wgt = new WNNFunctionWidget(nr.activation_fn, PanelWidth / 2);
					row->elementAt(0)->addWidget(fn_wgt);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					if(m_coordinator->display_mode() == DisplayMode::Structure)
					{
						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Output Range";
						label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
						label->setStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Theoretical " << get_function_range(nr.activation_fn.type);// .to_string();
						label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
						label->setStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Actual " << m_coordinator->neuron_io_range(nr.id).output;// .to_string(2);
						label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
						label->setStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					}

					if(m_coordinator->display_mode() == DisplayMode::State)
					{
						row = table->rowAt(row_idx++);
						ss.str("");
						ss << "Activation = " << std::setprecision(3) << state.neurons.at(nr.id).activation;
						label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
						label->setStyleClass("fb_subelement");
						row->elementAt(0)->addWidget(label);
						row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
					}
				}
				else if(nr.type == NeuronType::Bias)
				{
					row = table->rowAt(row_idx++);
					ss.str("");
					ss << "Value = " << std::setprecision(2) << 1.0;	// TODO: Bias value
					label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
					label->setStyleClass("fb_subelement");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
				}
				else if(nr.type == NeuronType::Input && m_coordinator->display_mode() == DisplayMode::State)
				{
					row = table->rowAt(row_idx++);
					ss.str("");
					ss << "Value = " << std::setprecision(3) << state.neurons.at(nr.id).activation;
					label = new Wt::WText(Wt::WString::fromUTF8(ss.str(), true));
					label->setStyleClass("fb_subelement");
					row->elementAt(0)->addWidget(label);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
				}
			}

/*			void WNNInfoPane::on_hover_start(feedback_event const& evt)
			{
				m_info_table->clear();
				add_neuron_summary(evt.id, m_info_table);
			}

			void WNNInfoPane::on_hover_end(feedback_event const& evt)
			{
				m_info_table->clear();
				add_network_summary(m_info_table);
			}
*/
			void WNNInfoPane::on_status_changed(changed_event const& evt)
			{
				if(evt.test(
					changed_event::Flags::HoverNeuron |
					changed_event::Flags::SelectedNeuron
					))
				{
					m_info_table->clear();

					auto current = m_coordinator->current_neuron();
					if(current)
					{
						add_neuron_summary(*current, m_info_table);
					}
					else
					{
						add_network_summary(m_info_table);
					}
				}
			}

		}
	}
}


