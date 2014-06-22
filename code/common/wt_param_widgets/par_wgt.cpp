// par_wgt.cpp

#include "par_wgt.h"

#include "boolean_par_wgt.h"
#include "integer_par_wgt.h"
#include "realnum_par_wgt.h"
#include "string_par_wgt.h"
#include "enum_par_wgt.h"
#include "vector_par_wgt.h"
#include "random_par_wgt.h"

#include "container_par_wgt.h"

#include "pw_yaml.h"

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WFormWidget>
#include <Wt/WGroupBox>
#include <Wt/WHBoxLayout>

#include <boost/variant/static_visitor.hpp>


namespace prm
{
	YAML::Node find_subschema(YAML::Node const& schema, std::string const& name)
	{
		assert(schema.IsMap());

		if(schema["name"].as< std::string >() == name)
		{
			return schema;
		}
		else if(schema["type"].as< prm::ParamType >() == prm::ParamType::List)
		{
			for(auto const& e : schema["children"])
			{
				YAML::Node n = find_subschema(e, name);
				if(!n.IsNull())
				{
					return n;
				}
			}
		}

		return YAML::Node();
	}

	param_tree::param_tree()
	{}

	param_tree* param_tree::create(schema_provider_fn schema_provider)
	{
		auto default_schema = schema_provider(YAML::Node());

		param_tree* tree = new param_tree();
		param_wgt* root = param_wgt::create(default_schema, tree->m_schema_accessor);
		tree->setImplementation(root);
		tree->m_schema_accessor = [schema_provider, root](std::string const& subschema_name)
		{
			auto complete_schema = schema_provider(root->get_yaml_param());
			return find_subschema(complete_schema, subschema_name);
		};

		return tree;
	}

	YAML::Node param_tree::get_yaml_param()
	{
		auto root = static_cast<param_wgt const*>(implementation());
		return root->get_yaml_param();
	}

	void param_tree::set_from_yaml_param(YAML::Node const& vals)
	{
		auto root = static_cast<param_wgt*>(implementation());
		return root->set_from_yaml_param(vals);
	}


	void param_tree::param_wgt::initialize(YAML::Node const& script)//, schema_accessor_fn& schema_accessor)
	{
		m_id = script["name"].as< std::string >();

		m_base_impl = create_impl(script);// , schema_accessor);

		auto has_border = false;
		auto label = std::string{ "" };
		auto visual = script["visual"];
		if(visual)
		{
			has_border = visual["border"] && visual["border"].as< bool >();
			if(auto& label_node = visual["label"])
			{
				label = label_node.as< std::string >();
			}
		}

		if(has_border)
		{
			auto box = new Wt::WGroupBox(label);
			box->addWidget(m_base_impl);
			setImplementation(box);
		}
		else if(!label.empty())
		{
			auto container = new Wt::WContainerWidget();
			auto layout = new Wt::WHBoxLayout();
			layout->setContentsMargins(0, 0, 0, 0);
			container->setLayout(layout);
			Wt::WText* label_w = new Wt::WText(label);
			layout->addWidget(label_w);// , 0, Wt::AlignLeft | Wt::AlignTop);
			layout->addWidget(m_base_impl);// , 0, Wt::AlignLeft | Wt::AlignTop);
			layout->addStretch(1);
			setImplementation(container);
		}
		else
		{
			setImplementation(m_base_impl);
		}
	}
/*
	param_wgt* param_wgt::create(ParamType type, pw_options const& opt, std::string const& id, std::string const& label)
	{
		param_wgt* pw = nullptr;
		switch(type)
		{
			case ParamType::Boolean:
			pw = new boolean_par_wgt();
			break;

			case ParamType::Integer:
			pw = new integer_par_wgt();
			break;

			case ParamType::RealNumber:
			pw = new realnum_par_wgt();
			break;

			case ParamType::String:
			pw = new string_par_wgt();
			break;

			case ParamType::Enumeration:
			pw = new enum_par_wgt();
			break;

			case ParamType::Vector2:
			pw = new vector_par_wgt();
			break;

			case ParamType::List:
			pw = new container_par_wgt();
			break;
		}

		if(pw == nullptr)
		{
			return nullptr;
		}

		pw->m_id = id;
		pw->initialize(opt, label);
		return pw;
	}

	prm::par_constraints parse_constraints(YAML::Node const& constraints, prm::ParamType type)
	{
		assert(constraints.IsMap() || constraints.IsNull());
		switch(type)
		{
			case prm::Boolean:
			return prm::boolean_par_constraints();

			case prm::Integer:
			{
				prm::integer_par_constraints c;
				if(constraints["min"])
				{
					c.min = constraints["min"].as< int >();
				}
				if(constraints["max"])
				{
					c.max = constraints["max"].as< int >();
				}
				return c;
			}

			case prm::RealNumber:
			{
				prm::realnum_par_constraints c;
				if(constraints["min"])
				{
					c.min = constraints["min"].as< double >();
				}
				if(constraints["max"])
				{
					c.max = constraints["max"].as< double >();
				}
				return c;
			}

			case prm::String:
			return prm::string_par_constraints();

			case prm::Enumeration:
			{
				assert(constraints["values"]);
				YAML::Node const& vals = constraints["values"];
				assert(vals.IsSequence());

				prm::enum_par_constraints c;
				for(auto const& e : vals)
				{
					c.add(e.Scalar(), e.Scalar());
				}
				return c;
			}

			default:
			assert(false);
			return prm::par_null_constraints();
		}
	}
	*/
#if 0
	prm::param_wgt* create_widget_from_schema(
		YAML::Node const& schema,
		std::shared_ptr< std::function< YAML::Node(std::string) > > const& schema_accessor);

	void create_widget_contents_from_schema(
		prm::container_par_wgt* parent,
		std::string const& schema_name,
		YAML::Node const& schema,
		std::shared_ptr< std::function< YAML::Node(std::string) > > const& schema_accessor,
		size_t index = 0)
	{
		assert(schema.IsSequence());

		for(auto i = index; i < schema.size(); ++i)
		{
			auto e = schema[i];
			assert(e.IsMap());

			prm::ParamType type = e["type"].as< prm::ParamType >();
			if(type == prm::ParamType::List)
			{
				assert(e["children"]);

				parent->add_child(create_widget_from_schema(
					e,
					schema_accessor));
			}
			else
			{
				prm::param_wgt* w = prm::param_wgt::create(e);

				if(e["update"] && e["update"].Scalar() == "default")
				{
					w->connect_changed_handler([schema_name, parent, i, schema_accessor]
					{
						YAML::Node sub_schema = (*schema_accessor)(
							schema_name//, /* todo: should be id? */
							);

						parent->clear_children(i + 1);
						create_widget_contents_from_schema(parent, schema_name, sub_schema, schema_accessor, i + 1);
					});
				}
				parent->add_child(w);
			}
		}
	}

	prm::param_wgt* create_widget_from_schema(
		YAML::Node const& schema,
		std::shared_ptr< std::function< YAML::Node(std::string) > > const& schema_accessor)
	{
		assert(schema.IsMap() && schema["type"] && schema["type"].as< prm::ParamType >() == prm::ParamType::List);

		prm::container_par_wgt* wgt = (prm::container_par_wgt*)prm::param_wgt::create(schema);
		create_widget_contents_from_schema(wgt, schema["name"].as< std::string >(), schema["children"], schema_accessor);

		return wgt;
	}

	param_wgt* param_wgt::create(YAML::Node const& script)
	{
		auto type = script["type"].as< prm::ParamType >();
//		assert(is_leaf_type(type));

		param_wgt* pw = nullptr;
		switch(type)
		{
			case ParamType::Boolean:
			pw = new boolean_par_wgt();
			break;

			case ParamType::Integer:
			pw = new integer_par_wgt();
			break;

			case ParamType::RealNumber:
			pw = new realnum_par_wgt();
			break;

			case ParamType::String:
			pw = new string_par_wgt();
			break;

			case ParamType::Enumeration:
			pw = new enum_par_wgt();
			break;

			case ParamType::Vector2:
			pw = new vector_par_wgt();
			break;

			case ParamType::List:
			pw = new container_par_wgt();
			break;
		}

		if(pw == nullptr)
		{
			return nullptr;
		}

		//pw->m_id = id;
		pw->initialize(script);//opt, label);
		return pw;
	}
#endif

/*
	param_wgt* param_wgt::create(schema_provider_fn schema_provider)
	{
		YAML::Node schema = schema_provider(YAML::Node());

		// TODO: This rather hacky solution using shared_ptr is so that the schema accessor function object will be deallocated
		// automatically when the widget tree is destroyed. We could just pass copies around instead, however then the problem
		// is that just below, we can't create the lambda that will implement the access until we have the widget pointer
		// returned from create_widget_from_schema(), which needs to be passed the accessor...
		auto schema_accessor = std::make_shared< std::function< YAML::Node(std::string) > >();

		YAML::Node hack;
		hack["name"] = "root";
		hack["type"] = prm::ParamType::List;
		hack["children"] = schema;

		param_wgt* wgt = create_widget_from_schema(hack, schema_accessor);

		*schema_accessor = [wgt, schema_provider](std::string name)
		{
			YAML::Node param_vals = wgt->get_yaml_param();
			YAML::Node complete_schema = schema_provider(param_vals);
			return name == "root" ? complete_schema : find_subschema(complete_schema, name);
		};

		return wgt;
	}
*/

	param_tree::param_wgt* param_tree::param_wgt::create(YAML::Node const& schema, schema_accessor_fn& schema_accessor)
	{
		assert(schema.IsMap());

//		YAML::Node schema = schema_provider(YAML::Node());

//		auto schema_accessor = std::make_shared< std::function< YAML::Node(std::string) > >();

		auto type = schema["type"].as< prm::ParamType >();

		param_wgt* pw = nullptr;
		switch(type)
		{
			case ParamType::Boolean:
			pw = new boolean_par_wgt();
			break;

			case ParamType::Integer:
			pw = new integer_par_wgt();
			break;

			case ParamType::RealNumber:
			pw = new realnum_par_wgt();
			break;

			case ParamType::String:
			pw = new string_par_wgt();
			break;

			case ParamType::Enumeration:
			pw = new enum_par_wgt();
			break;

			case ParamType::Vector2:
			pw = new vector_par_wgt();
			break;

			case ParamType::Random:
			pw = new random_par_wgt();
			break;

			case ParamType::List:
			pw = new container_par_wgt(schema_accessor);
			break;
		}

		if(pw == nullptr)
		{
			return nullptr;
		}

		//pw->set_parent(?);
		pw->initialize(schema);// , schema_accessor);
		return pw;
	}

	void param_tree::param_wgt::connect_changed_handler(std::function< void() > const& handler)
	{
		Wt::WFormWidget* fw = dynamic_cast<Wt::WFormWidget*>(m_base_impl);
		if(fw)
		{
			fw->changed().connect(std::bind(handler));
		}
	}

	class yaml_param_visitor: public boost::static_visitor< YAML::Node >
	{
	public:
		template < typename T >
		inline YAML::Node operator() (T const& val) const
		{
			return YAML::Node(val);
		}

		inline YAML::Node operator() (std::vector< prm::param > const& p) const
		{
			// Should be dealt with by virtual override of get_yaml_param() in container_par_wgt.cpp
			assert(false);
			return YAML::Node();
		}
	};

	YAML::Node param_tree::param_wgt::get_yaml_param() const
	{
		return boost::apply_visitor(yaml_param_visitor(), get_param());
	}

	std::string param_tree::param_wgt::get_yaml_script() const
	{
		return YAML::Dump(get_yaml_param());
	}

	void param_tree::param_wgt::set_from_yaml_param(YAML::Node const& vals)
	{
		// TODO:
	}
}




