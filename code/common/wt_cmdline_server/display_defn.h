// display_defn.h

#ifndef __WB_WTDISP_DISPLAY_DEFN
#define __WB_WTDISP_DISPLAY_DEFN

#include <boost/any.hpp>

#include <string>


class display_base;

class display_defn
{
public:
	virtual std::string name() const = 0;
	virtual display_base* create(boost::any const& data) const = 0;
};



#endif


