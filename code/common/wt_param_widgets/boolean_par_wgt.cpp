// boolean_par_wgt.cpp

#include "boolean_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WCheckBox>

#include <boost/variant/get.hpp>


namespace prm
{
	// TODO: Multiple implementations (eg. combo box based), which can be selected through options of param_wgt::create()
	class boolean_par_wgt::impl: public Wt::WCheckBox
	{
	public:
		impl(YAML::Node const& script)
		{
			if(auto& def = script["default"])
			{
				setChecked(def.as< bool >());
			}
		}
	};

	Wt::WWidget* boolean_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
		return m_impl;
	}

	param boolean_par_wgt::get_param() const
	{
		return m_impl->isChecked();
	}

	void boolean_par_wgt::set_from_param(param const& p)
	{
		m_impl->setChecked(boost::get< bool >(p));
	}
}



