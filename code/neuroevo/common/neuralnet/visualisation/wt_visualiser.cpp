// wt_visualiser.cpp

#include "wt_visualiser.h"
#include "wt_nndisplay.h"
#include "wt_controlpanel.h"
#include "wt_infopane.h"

#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WScrollArea>


namespace nnet {
	namespace vis {
		namespace wt {

			WNNVisualiser::WNNVisualiser()
			{
				m_coordinator = std::make_shared< coordinator >();

				auto vlayout = new Wt::WVBoxLayout;
				vlayout->setContentsMargins(0, 0, 0, 0);
				vlayout->setSpacing(0);
				setLayout(vlayout);

				auto control_panel = new WNNControlPanel(m_coordinator);
				vlayout->addWidget(control_panel, 0, Wt::AlignTop);

				auto hlayout = new Wt::WHBoxLayout;
				hlayout->setContentsMargins(0, 0, 0, 0);
				hlayout->setSpacing(8);
				vlayout->addLayout(hlayout, 1);

				auto display_scroll = new Wt::WScrollArea();
				auto display_container = new Wt::WContainerWidget();
				auto display_layout = new Wt::WVBoxLayout;
				display_container->setLayout(display_layout);
				auto display = new WNNDisplay(m_coordinator);
				display_layout->addWidget(display, Wt::AlignLeft | Wt::AlignTop);
				display_scroll->setWidget(display_container);
				hlayout->addWidget(display_scroll, 1);

				auto info_scroll = new Wt::WScrollArea();
//				info_scroll->setHorizontalScrollBarPolicy(Wt::WScrollArea::ScrollBarPolicy::ScrollBarAlwaysOff);
				info_scroll->setMinimumSize(220, Wt::WLength::Auto);
				auto infopane = new WNNInfoPane(m_coordinator);
				info_scroll->setWidget(infopane);
				hlayout->addWidget(info_scroll);// , 0, Wt::AlignRight);
			}

			void WNNVisualiser::set_neural_net(std::shared_ptr< i_neuralnet > net)
			{
				m_coordinator->set_network(std::move(net));
			}

		}
	}
}


