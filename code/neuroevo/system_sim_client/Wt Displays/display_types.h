// display_types.h

#ifndef __WB_WTDISP_TYPES_H
#define __WB_WTDISP_TYPES_H

#include "system_sim/system_drawer.h"

#include <boost/any.hpp>

#include <vector>
#include <memory>
#include <mutex>


enum class DisplayType {
	Chart,
	Drawer,
};

struct display_data
{
	DisplayType type;
	boost::any data;
};


struct chart_dd
{
	std::vector< boost::any > x_vals;

	struct series_data
	{
		std::string name;
		std::vector< double > y_vals;
	};

	std::vector< series_data > series;

	bool reload;	// if true, chart reloads all the data; otherwise, just adds on any beyond its existing data point count

	chart_dd(): reload{ false }
	{}
};

struct drawer_dd
{
	std::mutex* mx;
	std::shared_ptr< sys::i_system_drawer > drawer;
};



#endif


