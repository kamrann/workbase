// container_par_wgt.cpp

#include "container_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>

#include <boost/variant/get.hpp>


namespace prm
{
	container_par_wgt::container_par_wgt(schema_accessor_fn& schema_accessor): m_schema_accessor(schema_accessor)
	{

	}

	void container_par_wgt::create_child_widgets(
		std::string const& schema_name,
		YAML::Node const& schema,
		size_t start_index)
	{
		assert(schema.IsSequence());

		for(auto index = start_index; index < schema.size(); ++index)
		{
			auto e = schema[index];
			assert(e.IsMap());

			param_wgt* child = param_wgt::create(e, m_schema_accessor);

			if(e["update"] && e["update"].Scalar() == "default")
			{
				child->connect_changed_handler([this, schema_name, index]
				{
					YAML::Node sub_schema = m_schema_accessor(schema_name);

					clear_children(index + 1);
					create_child_widgets(schema_name, sub_schema["children"], index + 1);
				});
			}

			add_child(child);
		}
	}


	// TODO: Multiple implementations (eg. combo box based), which can be selected through options of param_wgt::create()
	class container_par_wgt::impl: public Wt::WContainerWidget
	{
	public:
		impl(YAML::Node const& script)
		{
			Layout style = Layout::Vertical;
			if(auto& visual = script["visual"])
			{
				if(auto& layout = visual["layout"])
				{
					style = layout.as< Layout >();
				}
			}

			Wt::WBoxLayout* layout = nullptr;
			switch(style)
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
		}
	};

	Wt::WWidget* container_par_wgt::create_impl(YAML::Node const& script)//, schema_accessor_fn& schema_accessor)
	{
		m_impl = new impl(script);
		// Move into impl constructor?
		if(script["children"])
		{
			create_child_widgets(/*this, */script["name"].as< std::string >(), script["children"]);// , schema_accessor);
		}
		//
		return m_impl;
	}
	
	void container_par_wgt::add_child(param_wgt* w)
	{
		m_impl->layout()->addWidget(w);
	}
	
	void container_par_wgt::clear_children(size_t from_index)
	{
		auto layout = m_impl->layout();
		while(m_impl->count() > from_index)
		{
			auto child = m_impl->widget(from_index);
			layout->removeWidget(child);
			delete child;
		}
	}

	param container_par_wgt::get_param() const
	{
		std::vector< param > child_params;
		auto num = m_impl->count();
		for(auto i = 0; i < num; ++i)
		{
			Wt::WWidget* w = m_impl->widget(i);
			// TODO: For now just assuming that all children are param_wgts
			param_wgt* pw = (param_wgt*)w;
			child_params.push_back(pw->get_param());
		}
		return child_params;
	}

	YAML::Node container_par_wgt::get_yaml_param() const
	{
		YAML::Node node;
		auto num = m_impl->count();
		for(auto i = 0; i < num; ++i)
		{
			Wt::WWidget* w = m_impl->widget(i);
			// TODO: For now just assuming that all children are param_wgts
			param_wgt* pw = (param_wgt*)w;
			node[pw->get_id()] = pw->get_yaml_param();
		}
		return node;
	}

	void container_par_wgt::set_from_param(param const& p)
	{
		//m_impl->setText(boost::get< std::string >(p));
		// TODO: To implement. Clear children then use visitor to actually create new widgets???
	}
}



