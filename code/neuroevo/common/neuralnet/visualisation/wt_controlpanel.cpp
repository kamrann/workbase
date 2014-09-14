// wt_controlpanel.cpp

#include "wt_controlpanel.h"
#include "coordinator.h"

#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WMenu>
#include <Wt/WPopupWidget>
#include <Wt/WText>
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
				auto style = decorationStyle();
				style.setBackgroundColor(Wt::WColor(200, 200, 200));
				setDecorationStyle(style);

				auto vlayout = new Wt::WVBoxLayout;
				setLayout(vlayout);

				auto hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);
				hlayout->addWidget(new Wt::WText("Neurons"), 0, Wt::AlignLeft);
				// TODO: Auto check

				auto slider = new Wt::WSlider;
				slider->resize(Wt::WLength::Auto, 30);
				slider->setMinimum(0);
				auto const MaxScale = 10;
				slider->setMaximum(MaxScale);
				int pos = (int)(m_coordinator->zoom_level() * MaxScale);
				slider->setValue(pos);
				slider->valueChanged().connect(std::bind([=]
				{
					auto pos = slider->value();
					double scale = (double)pos / MaxScale;
					m_coordinator->set_zoom_level(scale);
				}));
				vlayout->addWidget(slider);

				hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);
				hlayout->addWidget(new Wt::WText("Spacing"), 0, Wt::AlignLeft);
				// TODO: Auto check

				slider = new Wt::WSlider;
				slider->resize(Wt::WLength::Auto, 30);
				slider->setMinimum(0);
				//auto const MaxScale = 10;
				slider->setMaximum(MaxScale);
				pos = (int)(m_coordinator->spacing_scale() * MaxScale);
				slider->setValue(pos);
				slider->valueChanged().connect(std::bind([=]
				{
					auto pos = slider->value();
					double scale = (double)pos / MaxScale;
					m_coordinator->set_spacing_scale(scale);
				}));
				vlayout->addWidget(slider);
			}

		private:
			std::shared_ptr< coordinator > m_coordinator;
		};


		WNNControlPanel::WNNControlPanel(std::shared_ptr< coordinator > crd):
			m_coordinator(std::move(crd))
		{
			auto layout_ = new Wt::WHBoxLayout;
			setLayout(layout_);

			auto menu = new Wt::WMenu(Wt::Orientation::Horizontal);
			menu->addStyleClass("controlpanel");

			auto scale_item = new Wt::WMenuItem("Scale");
			menu->addItem(scale_item);

			auto scale_popup = new Wt::WPopupWidget(new WScaleControls(m_coordinator));
			scale_popup->setAnchorWidget(scale_item);
			scale_popup->setTransient(true);
			scale_popup->setDeleteWhenHidden(false);

			scale_item->triggered().connect(std::bind([=]
			{
				scale_popup->show();
			}));

			scale_popup->hidden().connect(std::bind([=]
			{
				menu->select(nullptr);
			}));

			menu->addItem(new Wt::WMenuItem("Test"));

			layout_->addWidget(menu, 0, Wt::AlignLeft);
			
			auto mode_box = new Wt::WComboBox;
			mode_box->addItem(DisplayModeStrings[(int)DisplayMode::Structure]);
			mode_box->addItem(DisplayModeStrings[(int)DisplayMode::State]);
			mode_box->setValueText(DisplayModeStrings[(int)m_coordinator->display_mode()]);
			layout_->addWidget(mode_box, 0, Wt::AlignRight);
		}

	}
}



