// string_par_wgt.cpp

#include "string_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WLineEdit>

#include <boost/variant/get.hpp>


namespace prm
{
	class string_par_wgt::default_impl:
		public param_tree::param_wgt_impl,
		public Wt::WLineEdit
	{
	public:
		default_impl(YAML::Node schema, bool readonly)
		{
			if(auto& def = schema["default"])
			{
				setText(def.as< std::string >());
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
			return param{ text().toUTF8() };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			setText(p.as< std::string >());
			return true;
		}
	};


	param_tree::param_wgt_impl* string_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;
		return new default_impl(schema, readonly);
	}
}



