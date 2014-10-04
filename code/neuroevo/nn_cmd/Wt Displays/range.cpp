// range.cpp

#include "range.h"

#include "util/interval_io.h"

#include "wt_custom_widgets/WMultiIntervalDisplay.h"

#include <Wt/WVBoxLayout>


range_display::range_display(range_dd const& dd)
{
	auto _layout = new Wt::WVBoxLayout;
	setLayout(_layout);

	for(auto const& r : dd)
	{
		auto mi_widget = new WMultiIntervalDisplay< nnet::range_t >{};
		mi_widget->set_multi_interval(r);
		_layout->addWidget(mi_widget, 0);
	}
}


