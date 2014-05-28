// integer_par_wgt.cpp

#include "integer_par_wgt.h"

#include <Wt/WSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class integer_par_wgt::impl: public Wt::WSpinBox
	{
	public:
		impl(integer_par_constraints const& c)
		{
			setMinimum(c.min);
			setMaximum(c.max);
		}
	};

	Wt::WWidget* integer_par_wgt::create_impl(pw_options const& opt)
	{
		m_impl = new impl(boost::get< integer_par_constraints >(opt));
		return m_impl;
	}

	param integer_par_wgt::get_param() const
	{
		return m_impl->value();
	}

	void integer_par_wgt::set_from_param(param const& p)
	{
		m_impl->setValue(boost::get< int >(p));
	}
}



