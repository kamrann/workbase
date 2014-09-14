// repeat_par_wgt.h

#ifndef __WB_REPEAT_PARAM_WIDGET_H
#define __WB_REPEAT_PARAM_WIDGET_H

#include "par_wgt.h"


namespace prm
{
	class repeat_par_wgt
	{
	public:
		static std::array< std::string, 1 > const events;

	public:
		static param_tree::param_wgt_impl* create(
			YAML::Node schema,
			qualified_path const& id,
			param_tree* tree,
			param_wgt_impl::options_t options = param_wgt_impl::Default);

	private:
		class default_impl;
	};
}


#endif


