// multisel_par_wgt.cpp

#include "multisel_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WSelectionBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class multisel_par_wgt::impl: public Wt::WSelectionBox
	{
	public:
		impl(YAML::Node const& script)
		{
			size_t idx = 0;
			for(auto const& v : script["constraints"]["values"])
			{
				// TODO: Separate label and value strings
				addItem(v.as< std::string >());
				model()->setData(idx, 0, v.as< std::string >(), Wt::UserRole);
				++idx;
			}
		}
	};

	Wt::WWidget* multisel_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
		return m_impl;
	}

	param multisel_par_wgt::get_param() const
	{
		todo
		int idx = m_impl->currentIndex();
		return m_impl->model()->data(idx, 0, Wt::UserRole);
	}

	void multisel_par_wgt::set_from_param(param const& p)
	{
		todo
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



