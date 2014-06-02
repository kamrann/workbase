// vector_par_wgt.cpp

#include "vector_par_wgt.h"

#include <Wt/WContainerWidget>
#include <Wt/WDoubleSpinBox>//WLineEdit>

#include <boost/variant/get.hpp>


namespace prm
{
	class vector_par_wgt::impl: public Wt::WContainerWidget
	{
	public:
		impl(YAML::Node const& script)
		{
			m_x_box = new Wt::WDoubleSpinBox(this);
			m_y_box = new Wt::WDoubleSpinBox(this);

			if(auto def = script["default"])
			{
				set(def.as< vec2 >());
			}
		}

		vec2 get_value() const
		{
			return vec2(m_x_box->value(), m_y_box->value());
		}

		void set(vec2 const& v)
		{
			m_x_box->setValue(v[0]);
			m_y_box->setValue(v[1]);
		}

	protected:
		Wt::WDoubleSpinBox* m_x_box;
		Wt::WDoubleSpinBox* m_y_box;
	};

	Wt::WWidget* vector_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
		return m_impl;
	}

	param vector_par_wgt::get_param() const
	{
		return m_impl->get_value();
	}

	void vector_par_wgt::set_from_param(param const& p)
	{
		m_impl->set(boost::get< vec2 >(p));
	}
}



