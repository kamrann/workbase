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
				setLayout(vlayout);

				auto control_panel = new WNNControlPanel(m_coordinator);
				vlayout->addWidget(control_panel, 0, Wt::AlignTop);

				auto hlayout = new Wt::WHBoxLayout;
				vlayout->addLayout(hlayout);

				auto display_scroll = new Wt::WScrollArea();
				auto display_container = new Wt::WContainerWidget();
				display_container->setLayout(new Wt::WVBoxLayout());
				auto display = new WNNDisplay(m_coordinator);
				display_container->layout()->addWidget(display);
				display_scroll->setWidget(display_container);
				hlayout->addWidget(display_scroll, 1);

				auto infopane = new WNNInfoPane(m_coordinator);
				hlayout->addWidget(infopane, 0, Wt::AlignRight);
			}

			void WNNVisualiser::set_neural_net(std::shared_ptr< i_neuralnet > net)
			{
				m_coordinator->set_network(std::move(net));
			}

		}
	}
}


