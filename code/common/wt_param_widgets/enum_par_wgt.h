// enum_par_wgt.h

#ifndef __WB_ENUM_PARAM_WIDGET_H
#define __WB_ENUM_PARAM_WIDGET_H

#include "par_wgt.h"


namespace prm
{
	class enum_par_wgt:
		public param_wgt
	{
	public:
		virtual param get_param() const;
		virtual void set_from_param(param const& p);

	protected:
		virtual Wt::WWidget* create_impl(YAML::Node const& script);

	protected:
		class impl;
		impl* m_impl;
	};
}


#endif


