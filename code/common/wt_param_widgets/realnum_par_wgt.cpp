// realnum_par_wgt.cpp

#include "realnum_par_wgt.h"

#include <Wt/WDoubleSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class realnum_par_wgt::impl: public Wt::WDoubleSpinBox
	{
	public:
		impl(realnum_par_constraints const& c)
		{
			setMinimum(c.min);
			setMaximum(c.max);
		}
	};

	Wt::WWidget* realnum_par_wgt::create_impl(pw_options const& opt)
	{
		m_impl = new impl(boost::get< realnum_par_constraints >(opt));
		return m_impl;
	}

	param realnum_par_wgt::get_param() const
	{
		return m_impl->value();
	}

	void realnum_par_wgt::set_from_param(param const& p)
	{
		m_impl->setValue(boost::get< double >(p));
	}
}



