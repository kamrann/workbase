// string_par_wgt.h

#ifndef __WB_STRING_PARAM_WIDGET_H
#define __WB_STRING_PARAM_WIDGET_H

#include "par_wgt.h"


namespace prm
{
	class string_par_wgt
	{
	public:
		static param_tree::param_wgt_impl* create(YAML::Node schema, param_wgt_impl::options_t options = param_wgt_impl::Default);

	private:
		class default_impl;
	};

}


#endif


