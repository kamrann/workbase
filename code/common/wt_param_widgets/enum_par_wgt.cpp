// enum_par_wgt.cpp

#include "enum_par_wgt.h"
#include "pw_yaml.h"

#include "params/param_yaml.h"
#include "params/unspecified.h"

#include <Wt/WComboBox>
#include <Wt/WSelectionBox>

#include <boost/variant/get.hpp>


namespace prm
{
	// Single selection implementation
	class enum_par_wgt::single_impl:
		public param_tree::param_wgt_impl,
		public Wt::WComboBox
	{
	public:
		single_impl(YAML::Node schema, bool readonly, bool enable_unspecified)
		{
			size_t idx = 0;
			if(enable_unspecified)
			{
				addItem(unspecified);
				model()->setData(idx, 0, unspecified, Wt::UserRole);
				++idx;
			}
			for(auto const& v : schema["constraints"]["values"])
			{
				addItem(v["label"].as< std::string >());
				model()->setData(idx, 0, v["id"].as< std::string >(), Wt::UserRole);
				++idx;
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
			int idx = currentIndex();
			auto data = model()->data(idx, 0, Wt::UserRole);
			auto as_string = boost::any_cast<std::string>(data);
			auto as_any = boost::any(std::vector< std::string >({ as_string }));
			return param{ enum_param_val(as_any) };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			auto val = p.as< enum_param_val >();
			auto as_list = boost::any_cast<std::vector< std::string >>(val.contents);
			auto as_string = as_list.front();

			setCurrentIndex(-1);
			bool found = false;
			for(size_t idx = 0; idx < count(); ++idx)
			{
				auto data = model()->data(idx, 0, Wt::UserRole);
				auto data_as_string = boost::any_cast<std::string>(data);
				if(as_string == data_as_string)
				{
					setCurrentIndex(idx);
					found = true;
					break;
				}
			}

			return found;
		}
	};

	// Multi-select implementation
	class enum_par_wgt::multi_impl:
		public param_tree::param_wgt_impl,
		public Wt::WSelectionBox
	{
	public:
		multi_impl(YAML::Node schema, bool readonly)
		{
			size_t idx = 0;
			for(auto const& v : schema["constraints"]["values"])
			{
				addItem(v["label"].as< std::string >());
				model()->setData(idx, 0, v["id"].as< std::string >(), Wt::UserRole);
				++idx;
			}
			// TODO: Custom widget to allow control over min and max number of items selectable
			// Maybe drop down version too.
			setSelectionMode(Wt::SelectionMode::ExtendedSelection);
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
			auto& sel = selectedIndexes();
			std::vector< std::string > as_list;
			for(auto idx : sel)
			{
				auto data = model()->data(idx, 0, Wt::UserRole);
				as_list.emplace_back(boost::any_cast<std::string>(data));
			}
			auto as_any = boost::any(as_list);
			return param{ enum_param_val(as_any) };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			auto val = p.as< enum_param_val >();
			auto as_list = boost::any_cast<std::vector< std::string >>(val.contents);
			std::set< std::string > as_set{ as_list.begin(), as_list.end() };

			std::set< int > sel;
			for(size_t idx = 0; idx < count(); ++idx)
			{
				auto data = model()->data(idx, 0, Wt::UserRole);
				auto data_as_string = boost::any_cast<std::string>(data);
				if(as_set.find(data_as_string) != as_set.end())
				{
					sel.insert(idx);
				}
			}

			setSelectedIndexes(sel);
			return sel.size() == as_set.size();
		}
	};


	param_tree::param_wgt_impl* enum_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;

		size_t min_sel = 1, max_sel = 1;
		if(schema["constraints"]["minsel"])
		{
			min_sel = schema["constraints"]["minsel"].as< size_t >();
		}
		if(schema["constraints"]["maxsel"])
		{
			max_sel = schema["constraints"]["maxsel"].as< size_t >();
		}

		if(/*min_sel == 1 &&*/ max_sel == 1)
		{
			return new single_impl(schema, readonly, min_sel == 0);
		}
		else
		{
			return new multi_impl(schema, readonly);
		}
	}
}



