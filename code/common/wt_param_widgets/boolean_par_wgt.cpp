// boolean_par_wgt.cpp

#include "boolean_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WCheckBox>

#include <boost/variant/get.hpp>


namespace prm
{
	// Check box implementation
	class boolean_par_wgt::check_impl:
		public param_tree::param_wgt_impl,
		public Wt::WCheckBox
	{
	public:
		check_impl(YAML::Node schema, bool readonly)
		{
			if(auto& def = schema["default"])
			{
				setChecked(def.as< bool >());
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
//			param name{ get_id().leaf().name() };
			return param{ isChecked() };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			setChecked(p.as< bool >());
			return true;
		}
	};


	param_tree::param_wgt_impl* boolean_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		// TODO: Multiple implementations (eg. combo box based), which can be selected through options of param_wgt::create()
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >();//options & param_wgt_impl::ReadOnly != 0;
		return new check_impl(schema, readonly);
	}
}


