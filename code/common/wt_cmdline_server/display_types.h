// display_types.h

#ifndef __WB_WTDISP_TYPES_H
#define __WB_WTDISP_TYPES_H

#include <boost/any.hpp>

#include <string>


struct display_data
{
	std::string type;
	boost::any data;
};



#endif


