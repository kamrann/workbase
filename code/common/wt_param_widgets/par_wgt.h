// par_wgt.h

#ifndef __WB_PARAM_WIDGET_H
#define __WB_PARAM_WIDGET_H

#include "params/param.h"
#include "params/param_constraints.h"

#include <Wt/WCompositeWidget>


namespace prm
{
	typedef par_constraints pw_options;

	class param_wgt:
		public Wt::WCompositeWidget
	{
	public:
		param_wgt(): m_base_impl(nullptr)
		{}
		void initialize(pw_options const& opt, std::string const& label);

	public:
		static param_wgt* create(ParamType type, pw_options const& opt, std::string const& label = "");

	public:
		virtual void connect_changed_handler(boost::function< void() > const& handler);
		virtual param get_param() const = 0;
		virtual void set_from_param(param const& p) = 0;

	protected:
		virtual Wt::WWidget* create_impl(pw_options const& opt) = 0;

	protected:
		Wt::WWidget* m_base_impl;
	};
}


#endif


