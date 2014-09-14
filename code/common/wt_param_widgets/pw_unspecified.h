// pw_unspecified.h

#ifndef __WB_PW_UNSPECIFIED_H
#define __WB_PW_UNSPECIFIED_H

#include <yaml-cpp/yaml.h>

#include <string>


namespace prm
{
	// TODO: Placeholder until use controls with built in 'unspecified' functionality
	std::string const unspecified = "<unspecified>";

	struct is_unspecified
	{
		is_unspecified(): val(false) {}
		is_unspecified(bool v): val(v) {}
		operator bool() const
		{
			return val;
		}

		bool val;
	};
	//
}



#endif


