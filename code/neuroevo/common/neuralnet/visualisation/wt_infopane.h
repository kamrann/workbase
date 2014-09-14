// wt_infopane.h

#ifndef __WB_NN_WT_INFOPANE_H
#define __WB_NN_WT_INFOPANE_H

#include "../interface/basic_types.h"

#include <Wt/WContainerWidget>

#include <memory>


namespace Wt {
	class WTable;
}

namespace nnet {
	namespace vis {

		class coordinator;
		struct feedback_event;

		namespace wt {

			class WNNInfoPane:
				public Wt::WContainerWidget
			{
			public:
				WNNInfoPane(std::shared_ptr< coordinator > crd);

			protected:
				void process_feedback(feedback_event const& evt);
				void on_hover_start(feedback_event const& evt);
				void on_hover_end(feedback_event const& evt);

				void add_network_summary(Wt::WTable* table);
				void add_neuron_summary(neuron_id id, Wt::WTable* table);

			protected:
				Wt::WTable* m_info_table;

				std::shared_ptr< coordinator > m_coordinator;
			};

		}
	}
}


#endif


