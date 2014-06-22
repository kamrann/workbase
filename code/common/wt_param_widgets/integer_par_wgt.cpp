// integer_par_wgt.cpp

#include "integer_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class integer_par_wgt::impl: public Wt::WSpinBox
	{
	public:
		impl(YAML::Node const& script)
		{
			setMinimum(std::numeric_limits< int >::min());
			setMaximum(std::numeric_limits< int >::max());
			if(auto& c = script["constraints"])
			{
				if(auto& min = c["min"])
				{
					setMinimum(min.as< int >());
				}
				if(auto& max = c["max"])
				{
					setMaximum(max.as< int >());
				}
			}
			if(auto& def = script["default"])
			{
				setValue(def.as< int >());
			}
		}
	};

	Wt::WWidget* integer_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);// boost::get< integer_par_constraints >(opt));
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



