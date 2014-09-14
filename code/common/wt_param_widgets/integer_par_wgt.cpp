// integer_par_wgt.cpp

#include "integer_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class integer_par_wgt::spinbox_impl:
		public param_tree::param_wgt_impl,
		public Wt::WSpinBox
	{
	public:
		spinbox_impl(YAML::Node schema, bool readonly)
		{
			setMinimum(std::numeric_limits< int >::min());
			setMaximum(std::numeric_limits< int >::max());
			if(auto& c = schema["constraints"])
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
			if(auto& def = schema["default"])
			{
				setValue(def.as< int >());
			}

			setReadOnly(readonly);
		}

		virtual Wt::WWidget* get_wt_widget()
		{
			return this;
		}

		virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler)
		{
			// TODO: others
			return changed().connect(std::bind(handler));
		}

		virtual param get_locally_instantiated_yaml_param(bool) const
		{
			return param{ value() };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			setValue(p.as< int >());
			return true;
		}
	};


	param_tree::param_wgt_impl* integer_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;
		return new spinbox_impl(schema, readonly);
	}
}



