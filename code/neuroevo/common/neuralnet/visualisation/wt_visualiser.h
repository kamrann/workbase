// wt_visualiser.h

#ifndef __WB_NN_WT_VISUALISER_H
#define __WB_NN_WT_VISUALISER_H

#include "coordinator.h"

#include <Wt/WContainerWidget>

#include <memory>


namespace nnet {
	namespace vis {
		namespace wt {

			class WNNVisualiser:
				public Wt::WContainerWidget
			{
			public:
				WNNVisualiser();

			public:
				void set_neural_net(std::shared_ptr< i_neuralnet > net);

			protected:
				std::shared_ptr< coordinator > m_coordinator;
			};

		}
	}
}


#endif


