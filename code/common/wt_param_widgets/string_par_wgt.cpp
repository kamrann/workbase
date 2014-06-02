// string_par_wgt.cpp

#include "string_par_wgt.h"

#include <Wt/WLineEdit>

#include <boost/variant/get.hpp>


namespace prm
{
	// TODO: Multiple implementations (eg. combo box based), which can be selected through options of param_wgt::create()
	class string_par_wgt::impl: public Wt::WLineEdit
	{
	public:
		impl(YAML::Node const& script)
		{
			if(auto def = script["default"])
			{
				setText(def.as< std::string >());
			}
		}
	};

	Wt::WWidget* string_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
		return m_impl;
	}

	param string_par_wgt::get_param() const
	{
		return m_impl->text().toUTF8();
	}

	void string_par_wgt::set_from_param(param const& p)
	{
		m_impl->setText(boost::get< std::string >(p));
	}
}



