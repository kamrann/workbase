// container_par_wgt.cpp

#include "container_par_wgt.h"
#include "pw_yaml.h"

#include "params/param_yaml.h"

#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>

#include <boost/variant/get.hpp>


namespace prm
{
	class container_par_wgt::default_impl:
		public param_tree::param_wgt_impl,
		public Wt::WContainerWidget
	{
	public:
		default_impl(YAML::Node schema, qualified_path const& id, param_tree* tree)
		{
			m_layout_orientation = Layout::Vertical;
			if(auto& visual = schema["visual"])
			{
				if(auto& layout = visual["layout"])
				{
					m_layout_orientation = layout.as< Layout >();
				}
			}

			Wt::WBoxLayout* layout = nullptr;
			switch(m_layout_orientation)
			{
				case Horizontal:
				layout = new Wt::WHBoxLayout();
				layout->setSpacing(5);
				break;

				default:
				layout = new Wt::WVBoxLayout();
				layout->setSpacing(3);
				break;
			}

			layout->setContentsMargins(0, 0, 0, 0);
			setLayout(layout);

			if(schema["children"])
			{
//				auto readonly = schema["readonly"] && schema["readonly"].as< bool >();
				create_child_widgets(schema["name"].as< std::string >(), schema["children"], id, tree, false);// readonly);
			}
		}

		virtual Wt::WWidget* get_wt_widget()
		{
			return this;
		}

		virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler)
		{
			// TODO:
			if(type == "changed")
			{
				return m_changed_event.connect(std::bind(handler));
			}

			return Wt::Signals::connection();
		}

		virtual param get_locally_instantiated_yaml_param(bool recursively_instantiate) const;
		virtual bool update_impl_from_yaml_param(param const& p);

	protected:
		void create_child_widgets(
			std::string const& schema_name,
			YAML::Node const& schema,
			qualified_path const& id,
			param_tree* tree,
			bool readonly,
			size_t start_index = 0)
		{
			assert(schema.IsSequence());

			auto layout_ = static_cast<Wt::WBoxLayout*>(layout());
			Wt::WBoxLayout* sublayout = nullptr;
			bool previous_unbordered = false;
			for(auto index = start_index; index < schema.size(); ++index)
			{
				auto e = schema[index];
				assert(e.IsMap());

//				bool child_readonly = e["readonly"] && e["readonly"].as< bool >();
//				e["readonly"] = child_readonly || readonly;
				auto child = tree->create_widget(e, id);

				bool standalone =
					e["visual"]["border"] && e["visual"]["border"].as< bool >() ||
					!is_leaf_type(e["type"].as< ParamType >());
				if(standalone)
				{
					// Add directly to layout
					layout_->addWidget(child, 0, Wt::AlignLeft | Wt::AlignTop);
					sublayout = nullptr;
				}
				else
				{
					if(sublayout == nullptr)
					{
						sublayout = m_layout_orientation == Layout::Vertical ?
							(Wt::WBoxLayout*)new Wt::WVBoxLayout :
							(Wt::WBoxLayout*)new Wt::WHBoxLayout;
						layout_->addLayout(sublayout, 0, Wt::AlignLeft | Wt::AlignTop);
					}
					sublayout->addWidget(child);
				}

				// Emit a changed signal any time a child does.
				child->connect_handler("changed", [=]
				{
					tree->find_widget(id)->notify(child->get_id(), 0);

					m_changed_event.emit(changed());
				});
			}
		}

	protected:
		Layout m_layout_orientation;

		struct changed
		{};

		Wt::Signal< changed > m_changed_event;
	};

/*
	YAML::Node condense_single_entry_map_sequence(YAML::Node const& seq)
	{
		YAML::Node mp;
		for(auto const& n : seq)
		{
			assert(n.IsMap() && n.size() == 1);

			auto const& entry = *n.begin();
			mp[entry.first] = entry.second;
		}
		return mp;
	}
*/
	param container_par_wgt::default_impl::get_locally_instantiated_yaml_param(bool recursively_instantiate) const
	{
		param content = YAML::Load("[]");
		auto num = count();
		for(auto i = 0; i < num; ++i)
		{
			Wt::WWidget* w = widget(i);
			// TODO: For now just assuming that all children are param_wgts
			param_tree::param_wgt* pw = (param_tree::param_wgt*)w;
			content.push_back(pw->get_yaml_param(recursively_instantiate));
		}
		return content;
	}

	bool container_par_wgt::default_impl::update_impl_from_yaml_param(param const& p)
	{
		auto val_param = p;
		auto num_params = val_param.size();
		std::list< param > child_params{ val_param.begin(), val_param.end() };

		auto num_children = count();
		bool success = num_params == num_children;
		for(size_t i_child = 0; i_child < num_children; ++i_child)
		{
			Wt::WWidget* w = widget(i_child);
			// TODO: For now just assuming that all children are param_wgts
			param_tree::param_wgt* pw = (param_tree::param_wgt*)w;

			bool child_success = false;
			for(auto it = child_params.begin(); it != child_params.end(); ++it)
			{
				if(pw->set_from_yaml_param(*it))
				{
					child_success = true;
					child_params.erase(it);
					break;
				}
			}
			success = success && child_success;
		}
		return success;
	}


	param_tree::param_wgt_impl* container_par_wgt::create(
		YAML::Node schema,
		qualified_path const& id,
		param_tree* tree,
		param_wgt_impl::options_t options)
	{
		//bool readonly = options & param_wgt_impl::ReadOnly != 0;
		return new default_impl(schema, id, tree);
	}
}



