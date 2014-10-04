// display_types.h

#ifndef __WB_NNCMD_WTDISP_TYPES_H
#define __WB_NNCMD_WTDISP_TYPES_H

#include "neuralnet/interface/basic_types.h"

#include <boost/any.hpp>

#include <vector>


enum class DisplayType {
	Range,
};

struct display_data
{
	DisplayType type;
	boost::any data;
};

typedef std::vector< nnet::range_t > range_dd;


#endif


