// realnum_par_wgt.cpp

#include "realnum_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WDoubleSpinBox>

#include <boost/variant/get.hpp>


namespace prm
{
	class realnum_par_wgt::spinbox_impl:
		public param_tree::param_wgt_impl,
		public Wt::WDoubleSpinBox
	{
	public:
		spinbox_impl(YAML::Node schema, bool readonly)
		{
			setMinimum(-std::numeric_limits< double >::max());
			setMaximum(std::numeric_limits< double >::max());
			if(auto& c = schema["constraints"])
			{
				if(auto& min = c["min"])
				{
					setMinimum(min.as< double >());
				}
				if(auto& max = c["max"])
				{
					setMaximum(max.as< double >());
				}
			}
			if(auto& def = schema["default"])
			{
				setValue(def.as< double >());
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
			setValue(p.as< double >());
			return true;
		}
	};


	param_tree::param_wgt_impl* realnum_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;
		return new spinbox_impl(schema, readonly);
	}
}



