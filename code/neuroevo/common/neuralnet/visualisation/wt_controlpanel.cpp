// wt_controlpanel.cpp

#include "wt_controlpanel.h"
#include "coordinator.h"

#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
//#include <Wt/WMenu>
#include <Wt/WPushButton>
#include <Wt/WPopupWidget>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WSlider>
#include <Wt/WCSSDecorationStyle>


namespace nnet {
	namespace vis {

		//const size_t BarHeight = 50;

		// TODO: Move, since modes are defined in coordinator.h
		const char* const DisplayModeStrings[] = {
			"Structure",
			"State",
		};


		class WScaleControls:
			public Wt::WContainerWidget
		{
		public:
			WScaleControls(std::shared_ptr< coordinator > crd):
				m_coordinator(std::move(crd))
			{
				addStyleClass("controlpanelbase");

				auto vlayout = new Wt::WVBoxLayout;
				setLayout(vlayout);

				auto hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);
				hlayout->addWidget(new Wt::WText("Neuron Size"), 0, Wt::AlignLeft);
				// TODO: Auto check

				auto slider = new Wt::WSlider;
				slider->resize(Wt::WLength::Auto, 30);
				slider->setMinimumSize(160, 30);
				slider->setMinimum(0);
				auto const MaxScale = 10;
				slider->setMaximum(MaxScale);
				int pos = (int)(m_coordinator->neuron_scale() * MaxScale);
				slider->setValue(pos);
				slider->valueChanged().connect(std::bind([=]
				{
					auto pos = slider->value();
					double scale = (double)pos / MaxScale;
					m_coordinator->set_neuron_scale(scale);
				}));
				vlayout->addWidget(slider);

				hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);
				hlayout->addWidget(new Wt::WText("Neuron Spacing"), 0, Wt::AlignLeft);
				// TODO: Auto check

				slider = new Wt::WSlider;
				slider->resize(Wt::WLength::Auto, 30);
				slider->setMinimumSize(160, 30);
				slider->setMinimum(0);
				//auto const MaxScale = 10;
				slider->setMaximum(MaxScale);
				pos = (int)(m_coordinator->neuron_spacing() * MaxScale);
				slider->setValue(pos);
				slider->valueChanged().connect(std::bind([=]
				{
					auto pos = slider->value();
					double scale = (double)pos / MaxScale;
					m_coordinator->set_neuron_spacing(scale);
				}));
				vlayout->addWidget(slider);

				hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);
				hlayout->addWidget(new Wt::WText("Layer Spacing"), 0, Wt::AlignLeft);
				// TODO: Auto check

				slider = new Wt::WSlider;
				slider->resize(Wt::WLength::Auto, 30);
				slider->setMinimumSize(160, 30);
				slider->setMinimum(0);
				//auto const MaxScale = 10;
				slider->setMaximum(MaxScale);
				pos = (int)(m_coordinator->layer_spacing() * MaxScale);
				slider->setValue(pos);
				slider->valueChanged().connect(std::bind([=]
				{
					auto pos = slider->value();
					double scale = (double)pos / MaxScale;
					m_coordinator->set_layer_spacing(scale);
				}));
				vlayout->addWidget(slider);
			}

		private:
			std::shared_ptr< coordinator > m_coordinator;
		};

		class WDisplayControls:
			public Wt::WContainerWidget
		{
		public:
			WDisplayControls(std::shared_ptr< coordinator > crd):
				m_coordinator(std::move(crd))
			{
				addStyleClass("controlpanelbase");

				auto vlayout = new Wt::WVBoxLayout;
				setLayout(vlayout);

				auto connections_label = new Wt::WText("Connections");
				vlayout->addWidget(connections_label, 0, Wt::AlignLeft);

				auto connections = new Wt::WCheckBox("Show always");
				vlayout->addWidget(connections, 0, Wt::AlignLeft);
				connections->setChecked(m_coordinator->has_display_option(DisplayFlags::Connections));
				connections->changed().connect(std::bind([=]
				{
					m_coordinator->toggle_display_option(DisplayFlags::Connections, connections->isChecked());
				}));

				if(m_coordinator->display_mode() == DisplayMode::Structure)
				{
					auto colour_weights = new Wt::WCheckBox("Colour Weights");
					vlayout->addWidget(colour_weights, 0, Wt::AlignLeft);
					colour_weights->setChecked(m_coordinator->has_display_option(DisplayFlags::ColourWeights));
					colour_weights->changed().connect(std::bind([=]
					{
						m_coordinator->toggle_display_option(DisplayFlags::ColourWeights, colour_weights->isChecked());
					}));
				}

				if(m_coordinator->display_mode() == DisplayMode::State)
				{
					auto signal_gradients = new Wt::WCheckBox("Signal Gradients");
					vlayout->addWidget(signal_gradients, 0, Wt::AlignLeft);
					signal_gradients->setChecked(m_coordinator->has_display_option(DisplayFlags::SignalGradients));
					signal_gradients->changed().connect(std::bind([=]
					{
						m_coordinator->toggle_display_option(DisplayFlags::SignalGradients, signal_gradients->isChecked());
					}));
				}

				auto act_fn = new Wt::WCheckBox("Show Activation Function");
				vlayout->addWidget(act_fn, 0, Wt::AlignLeft);
				act_fn->setChecked(m_coordinator->has_display_option(DisplayFlags::ActivationFunction));
				act_fn->changed().connect(std::bind([=]
				{
					m_coordinator->toggle_display_option(DisplayFlags::ActivationFunction, act_fn->isChecked());
				}));
			}

		private:
			std::shared_ptr< coordinator > m_coordinator;
		};


		WNNControlPanel::WNNControlPanel(std::shared_ptr< coordinator > crd):
			m_coordinator(std::move(crd))
		{
			setMinimumSize(Wt::WLength::Auto, 40);	// NOTE: No idea why needed to add this,
													// but control bar was getting squished to 8 pixel height.
			addStyleClass("controlpanelbase controlpanel");

			auto layout_ = new Wt::WHBoxLayout;
			layout_->setContentsMargins(0, 0, 0, 0);
			layout_->setSpacing(0);
			setLayout(layout_);

			{
				auto scale_item = new Wt::WText("Scaling");
				scale_item->setStyleClass("controlpanelitem");
				layout_->addWidget(scale_item, 0, Wt::AlignLeft);

				scale_item->clicked().connect(std::bind([=]
				{
					auto scale_popup = new Wt::WPopupWidget(new WScaleControls(m_coordinator));
					scale_popup->setAnchorWidget(scale_item);
					scale_popup->setTransient(true);
					scale_popup->setDeleteWhenHidden(true);
					scale_popup->hidden().connect(std::bind([=]
					{
						scale_item->toggleStyleClass("active", false);
					}));

					scale_item->toggleStyleClass("active", true);
					scale_popup->show();
				}));
			}

			{
				auto display_item = new Wt::WText("Display");
				display_item->setStyleClass("controlpanelitem");
				layout_->addWidget(display_item, 0, Wt::AlignLeft);

				display_item->clicked().connect(std::bind([=]
				{
					auto display_popup = new Wt::WPopupWidget(new WDisplayControls(m_coordinator));
					display_popup->setAnchorWidget(display_item);
					display_popup->setTransient(true);
					display_popup->setDeleteWhenHidden(true);
					display_popup->hidden().connect(std::bind([=]
					{
						display_item->toggleStyleClass("active", false);
					}));

					display_item->toggleStyleClass("active", true);
					display_popup->show();
				}));
			}

			auto controls_cont = new Wt::WContainerWidget;
			auto controls_layout = new Wt::WHBoxLayout;
			controls_layout->setContentsMargins(4, 4, 4, 4);
			controls_layout->setSpacing(8);
			controls_cont->setLayout(controls_layout);
			layout_->addWidget(controls_cont, 1);// , 0, Wt::AlignmentFlag::AlignRight);

			auto mode_box = new Wt::WComboBox;
			mode_box->setStyleClass("cp_modebox");
			mode_box->setLineHeight(30);
			mode_box->addItem(DisplayModeStrings[(int)DisplayMode::Structure]);
			mode_box->addItem(DisplayModeStrings[(int)DisplayMode::State]);
			mode_box->setValueText(DisplayModeStrings[(int)m_coordinator->display_mode()]);
			controls_layout->addWidget(mode_box, 0, Wt::AlignRight);

			mode_box->changed().connect(std::bind([=]
			{
				auto mode = (DisplayMode)mode_box->currentIndex();
				m_coordinator->set_display_mode(mode);
			}));
		}

	}
}



