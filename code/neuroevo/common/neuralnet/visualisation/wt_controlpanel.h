// wt_controlpanel.h

#ifndef __WB_NN_WT_CONTROLPANEL_H
#define __WB_NN_WT_CONTROLPANEL_H

#include <Wt/WContainerWidget>


namespace nnet {
	namespace vis {

		class coordinator;

		class WNNControlPanel:
			public Wt::WContainerWidget
		{
		public:
			WNNControlPanel(std::shared_ptr< coordinator > crd);

		private:
			std::shared_ptr< coordinator > m_coordinator;
		};

	}
}


#endif


