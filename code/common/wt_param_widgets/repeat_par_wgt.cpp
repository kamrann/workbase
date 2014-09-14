// repeat_par_wgt.cpp

#include "repeat_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>


namespace prm
{
	std::array< std::string, 1 > const repeat_par_wgt::events = {
		"changed"
		// TODO: add, remove, modify contents, etc
	};


	class repeat_par_wgt::default_impl:
		public param_tree::param_wgt_impl,
		public Wt::WContainerWidget
	{
	public:
		default_impl(YAML::Node schema, qualified_path const& id, param_tree* tree):
			m_id(id),
			m_tree(tree)
		{
			// By starting from 1, we should force the YAML node to be a map, even when the keys are consecutive
			m_next_instance_num = 1;

			auto layout = new Wt::WVBoxLayout();
			layout->setSpacing(3);

			layout->setContentsMargins(0, 0, 0, 0);
			setLayout(layout);

			m_contents_name = schema["contents"].as< std::string >();
			m_readonly = schema["readonly"];

			if(!m_readonly)
			{
				create_expander();

			// TODO: Depend on script default, min, max
			//		add_instance();
			//
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

		param get_locally_instantiated_yaml_param(bool recursively_instantiate) const
		{
			param content = YAML::Load("{}");
			for(auto inst : m_instances)
			{
				auto inst_num = inst.first;
				auto pw = inst.second;
				content[inst_num] = pw->get_yaml_param(recursively_instantiate);
			}
			return content;
		}

		bool update_impl_from_yaml_param(param const& p)
		{
			// First need to clear existing instances
			auto layout_ = static_cast<Wt::WVBoxLayout*>(layout());
			auto count = instance_count();
			for(size_t i = 0; i < count; ++i)	// Leave expander intact
			{
				auto item = layout_->itemAt(i);
				layout_->removeItem(item);
				delete item;
			}
			m_instances.clear();

			bool success = true;
			m_next_instance_num = 1;
			auto inst_map = p;
			for(auto const& entry : inst_map)
			{
				auto inst_num = entry.first.as< int >();
				auto inst_param = entry.second;

				inst_num = m_next_instance_num;	// TODO: For now, ignoring the instance number of the given param...
				add_instance();
				auto inst_wgt = m_instances[inst_num];
				success = success && inst_wgt->set_from_yaml_param(inst_param);
			}

			// TODO: m_temp_event.emit(TempEvent());

			return success;
		}

		size_t instance_count() const
		{
			return m_instances.size();
		}

		param_tree::param_wgt* get_instance(unsigned int inst_num)
		{
			auto it = m_instances.find(inst_num);
			return it == m_instances.end() ? nullptr : it->second;
		}

		void create_expander();
		void add_instance();

	protected:
		qualified_path m_id;
		param_tree* m_tree;

		std::string m_contents_name;
		bool m_readonly;
		unsigned int m_next_instance_num;
		std::map< unsigned int, param_tree::param_wgt* > m_instances;

		struct changed
		{};

		Wt::Signal< changed > m_changed_event;
	};

	void repeat_par_wgt::default_impl::create_expander()
	{
		auto expander = new Wt::WPushButton();
		expander->setStyleClass("expander");

		expander->clicked().connect(std::bind([this]()
		{
			add_instance();
			m_changed_event.emit(changed());
		}));

		auto layout_ = static_cast<Wt::WBoxLayout*>(layout());
		layout_->addWidget(expander, 0, Wt::AlignLeft | Wt::AlignTop);
	}

	void repeat_par_wgt::default_impl::add_instance()
	{
		auto instance_num = m_next_instance_num++;

		auto child_prefix = m_id;
		child_prefix.leaf().set_index(instance_num);
		auto contents_id = child_prefix + m_contents_name;
		auto script = m_tree->get_schema(contents_id);
//		bool content_readonly = script["readonly"] && script["readonly"].as< bool >();
//		script["readonly"] = content_readonly || m_readonly;

		auto pw = m_tree->create_widget(script, child_prefix);

		// Emit a changed signal any time a child does.
		pw->connect_handler("changed", [=]
		{
			m_tree->find_widget(m_id)->notify(pw->get_id(), 0);

			m_changed_event.emit(changed());
		});

		auto instance_box = new Wt::WContainerWidget();
		auto instance_layout = new Wt::WHBoxLayout();
		instance_layout->setSpacing(3);
		instance_layout->setContentsMargins(0, 0, 0, 0);
		instance_box->setLayout(instance_layout);

		auto layout_ = static_cast<Wt::WVBoxLayout*>(layout());

		if(!m_readonly)
		{
			auto delete_btn = new Wt::WPushButton();
			delete_btn->setStyleClass("remover");
			instance_layout->addWidget(delete_btn, 0, Wt::AlignTop);

			delete_btn->clicked().connect(std::bind([=]
			{
				layout_->removeWidget(instance_box);
				delete instance_box;
				m_instances.erase(instance_num);
				m_changed_event.emit(changed());
			}));
		}

		instance_layout->addWidget(pw);
		layout_->insertWidget(instance_count(), instance_box, 0, Wt::AlignLeft | Wt::AlignTop);

		m_instances[instance_num] = pw;

		// TODO: Temp hack. Rather than have a name element as part of the schema for use as a reference
		// (YAML anchor/alias), it should be an option built into every param widget. Then we can specify 
		// how to give it a default name on a per widget basis, and it could be edited via expandable/collapsable
		// gui element, similar to or maybe via the drop down menu used for imports.
		{
			auto name_path = pw->get_id() + std::string{ "name" };
			auto name_wgt = m_tree->find_widget(name_path);
			if(name_wgt != nullptr)
			{
				// Has a name element
				std::string default_name = pw->get_id().leaf().to_string("") + " " + std::to_string(instance_num);
				param name_param;
				name_param[ParamNode::Name] = "name";
				name_param[ParamNode::Type] = ParamType::String;
				name_param[ParamNode::Value] = default_name;
				name_wgt->set_from_yaml_param(name_param);
			}
		}
		/////////////////////////
	}


	param_tree::param_wgt_impl* repeat_par_wgt::create(
		YAML::Node schema,
		qualified_path const& id,
		param_tree* tree,
		param_wgt_impl::options_t options)
	{
		//bool readonly = options & param_wgt_impl::ReadOnly != 0;
		return new default_impl(schema, id, tree);
	}
}



