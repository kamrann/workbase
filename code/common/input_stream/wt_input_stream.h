// wt_input_stream.h

#ifndef __WB_WT_INPUT_STREAM_H
#define __WB_WT_INPUT_STREAM_H

#include "input_stream.h"

#include <Wt/WGlobal>

#include <boost/signals2/connection.hpp>

#include <map>
#include <mutex>


namespace Wt {
	class WInteractWidget;
}


namespace input {

	class wt_input_stream:
		public input_stream
	{
	public:
		wt_input_stream();

		void attach_to_widget(Wt::WInteractWidget* wgt);
		void detach_from_widget(Wt::WInteractWidget* wgt);

	public:
		virtual void update() override
		{}

		virtual input_stream_access lock() const override;

	protected:

	protected:
		std::map< Wt::Key, btn_element_id > key_mp_;

		boost::signals2::connection key_down_conn_;
		boost::signals2::connection key_up_conn_;

		mutable std::mutex mtx_;
	};

}


#endif

