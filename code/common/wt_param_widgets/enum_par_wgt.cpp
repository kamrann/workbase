// enum_par_wgt.cpp

#include "enum_par_wgt.h"

#include <Wt/WComboBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class enum_par_wgt::impl: public Wt::WComboBox
	{
	public:
		impl(enum_par_constraints const& c)
		{
			size_t idx = 0;
			for(auto const& v : c.vals)
			{
				addItem(v.name);
				model()->setData(idx, 0, v.val, Wt::UserRole);
				++idx;
			}
		}
	};

	Wt::WWidget* enum_par_wgt::create_impl(pw_options const& opt)
	{
		m_impl = new impl(boost::get< enum_par_constraints >(opt));
		return m_impl;
	}

	param enum_par_wgt::get_param() const
	{
		int idx = m_impl->currentIndex();
		return m_impl->model()->data(idx, 0, Wt::UserRole);
	}

	void enum_par_wgt::set_from_param(param const& p)
	{
		Wt::WModelIndexList idx = m_impl->model()->match(
			m_impl->model()->index(0, 0),
			Wt::UserRole,
			boost::get< enum_param_val >(p),
			1
			);
		assert(!idx.empty());
		m_impl->setCurrentIndex(idx.front().row());
	}
}



