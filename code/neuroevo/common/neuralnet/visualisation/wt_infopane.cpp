// wt_infopane.cpp

#include "wt_infopane.h"
#include "wt_nndisplay.h"
#include "text_output.h"
#include "coordinator.h"
#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"

#include <Wt/WVBoxLayout>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WCSSDecorationStyle>


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
				setWidth(PanelWidth);

				m_info_table = new Wt::WTable;
				m_info_table->addStyleClass("outerfeedbacktable");
				vlayout->addWidget(m_info_table, 0, Wt::AlignTop);

				auto handler = [this](feedback_event const& evt)
				{
					process_feedback(evt);
				};
				m_coordinator->register_feedback_handler(handler);
			}

			void WNNInfoPane::process_feedback(feedback_event const& evt)
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

			void WNNInfoPane::add_network_summary(Wt::WTable* table)
			{
				auto net = m_coordinator->network();

				std::stringstream ss;
				std::fixed(ss);
				ss.precision(2);

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
			}

			void WNNInfoPane::add_neuron_summary(neuron_id id, Wt::WTable* table)
			{
				auto net = m_coordinator->network();
				auto nr_access = net->neuron_access();
				auto nr = nr_access->get_neuron_data(id);
				auto incoming = nr_access->get_incoming_connections(id);

				std::stringstream ss;
				std::fixed(ss);
				ss.precision(3);

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
				row->elementAt(0)->addWidget(new Wt::WText(ss.str()));
				row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

				row = table->rowAt(row_idx++);
				row->elementAt(0)->decorationStyle().setBorder(
					Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Top);

				if(has_inputs(nr.type))
				{
					// Incoming
					row = table->rowAt(row_idx++);
					label = new Wt::WText("Incoming Connections");
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
						row->elementAt(0)->addWidget(new Wt::WText(neuron_name(net, conn.src)));
						row->elementAt(0)->decorationStyle().setBorder(
							Wt::WBorder(Wt::WBorder::Solid, 1, Wt::WColor(200, 200, 200)), Wt::Side::Right);
						ss.str("");
						ss << conn.weight;
						row->elementAt(1)->addWidget(new Wt::WText(ss.str()));
						row->elementAt(1)->setContentAlignment(Wt::AlignRight);
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
					auto fn_wgt = new WNNFunctionWidget(nr.activation_fn, PanelWidth / 2);
					row->elementAt(0)->addWidget(fn_wgt);
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					ss.str("");
					ss << ActivationFnNames[(int)nr.activation_fn];
					row->elementAt(0)->addWidget(new Wt::WText(ss.str()));
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);

					row = table->rowAt(row_idx++);
					ss.str("");
					ss << "Output Range " << get_function_range(nr.activation_fn).to_string();
					row->elementAt(0)->addWidget(new Wt::WText(Wt::WString::fromUTF8(ss.str(), true)));
					row->elementAt(0)->setContentAlignment(Wt::AlignCenter);
				}
			}

			void WNNInfoPane::on_hover_start(feedback_event const& evt)
			{
				m_info_table->clear();
				add_neuron_summary(evt.id, m_info_table);
			}

			void WNNInfoPane::on_hover_end(feedback_event const& evt)
			{
				m_info_table->clear();
				add_network_summary(m_info_table);
			}

		}
	}
}


