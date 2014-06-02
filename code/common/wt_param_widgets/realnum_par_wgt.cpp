// realnum_par_wgt.cpp

#include "realnum_par_wgt.h"

#include <Wt/WDoubleSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class realnum_par_wgt::impl: public Wt::WDoubleSpinBox
	{
	public:
		impl(YAML::Node const& script)
		{
			setMinimum(-std::numeric_limits< double >::max());
			setMaximum(std::numeric_limits< double >::max());
			if(auto c = script["constraints"])
			{
				if(auto min = c["min"])
				{
					setMinimum(min.as< double >());
				}
				if(auto max = c["max"])
				{
					setMaximum(max.as< double >());
				}
			}
			if(auto def = script["default"])
			{
				setValue(def.as< double >());
			}
		}
	};

	Wt::WWidget* realnum_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
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



