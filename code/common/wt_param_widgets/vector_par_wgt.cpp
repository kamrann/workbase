// vector_par_wgt.cpp

#include "vector_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WContainerWidget>
#include <Wt/WDoubleSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class vector_par_wgt::default_impl:
		public param_tree::param_wgt_impl,
		public Wt::WContainerWidget
	{
	public:
		default_impl(YAML::Node schema, bool readonly)
		{
			m_x_box = new Wt::WDoubleSpinBox(this);
			m_y_box = new Wt::WDoubleSpinBox(this);

			if(auto& def = schema["default"])
			{
				update_impl_from_yaml_param(def);
			}

			m_x_box->setReadOnly(readonly);
			m_y_box->setReadOnly(readonly);
		}

		virtual Wt::WWidget* get_wt_widget()
		{
			return this;
		}

		virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler)
		{
			// TODO: !!
			return Wt::Signals::connection();
		}

		virtual param get_locally_instantiated_yaml_param(bool) const
		{
			return param{ vec2(m_x_box->value(), m_y_box->value()) };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			auto v = p.as< vec2 >();
			m_x_box->setValue(v[0]);
			m_y_box->setValue(v[1]);
			return true;
		}

	protected:
		Wt::WDoubleSpinBox* m_x_box;
		Wt::WDoubleSpinBox* m_y_box;
	};


	param_tree::param_wgt_impl* vector_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;
		return new default_impl(schema, readonly);
	}
}



