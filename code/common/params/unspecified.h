// unspecified.h

#ifndef __WB_PARAMS_UNSPECIFIED_H
#define __WB_PARAMS_UNSPECIFIED_H

#include <string>


namespace prm
{
	std::string const unspecified = "_unspecified_";

	// TODO: Probably need to build this into param type, so can support all types, not only enum
	inline bool is_unspecified(param const& p)
	{
		auto epv = extract_as< enum_param_val >(p);
		return epv.size() == 1 && epv.front() == unspecified;
	}

	struct is_unspecified_yaml
	{
		is_unspecified_yaml(): val(false) {}
		is_unspecified_yaml(bool v): val(v) {}

		operator bool() const
		{
			return val;
		}

		bool val;
	};
}



#endif


