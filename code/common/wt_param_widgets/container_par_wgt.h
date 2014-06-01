// container_par_wgt.h

#ifndef __WB_CONTAINER_PARAM_WIDGET_H
#define __WB_CONTAINER_PARAM_WIDGET_H

#include "par_wgt.h"


namespace prm
{
	class container_par_wgt:
		public param_wgt
	{
	public:
		virtual param get_param() const;
		virtual void set_from_param(param const& p);

		void add_child(param_wgt* w);
		void clear_children(size_t from_index = 0);

	protected:
		virtual Wt::WWidget* create_impl(pw_options const& opt);

	protected:
		class impl;
		impl* m_impl;
	};
}


#endif


