// sysutil.hpp

#ifndef __WB_SYSUTIL_H
#define __WB_SYSUTIL_H

#include "wb_string.hpp"

#include <boost/optional.hpp>

#include <cstdlib>


namespace wb {

	inline boost::optional< string > get_env(string const& name) 
	{ 
		char *var = std::getenv(epw_to_narrow(name).c_str()); 
		if(var != nullptr)
		{
			return narrow_to_epw(var);
		}
		else
		{
			return boost::none;
		}
	}

}


#endif


