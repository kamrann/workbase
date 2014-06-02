// par_wgt.cpp

#include "par_wgt.h"

#include "boolean_par_wgt.h"
#include "integer_par_wgt.h"
#include "realnum_par_wgt.h"
#include "string_par_wgt.h"
#include "enum_par_wgt.h"
#include "vector_par_wgt.h"

#include "container_par_wgt.h"

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WFormWidget>

#include <boost/variant/static_visitor.hpp>


namespace prm
{
/*	void param_wgt::initialize(pw_options const& opt, std::string const& label)
	{
		Wt::WText* label_w = new Wt::WText(label);
		m_base_impl = create_impl(opt);
		Wt::WContainerWidget* cont = new Wt::WContainerWidget();
		cont->addWidget(label_w);
		cont->addWidget(m_base_impl);
		setImplementation(cont);
	}
*/
	void param_wgt::initialize(YAML::Node const& script)
	{
		Wt::WText* label_w = new Wt::WText(script["name"].as< std::string >());
		m_base_impl = create_impl(script);
		Wt::WContainerWidget* cont = new Wt::WContainerWidget();
		cont->addWidget(label_w);
		cont->addWidget(m_base_impl);
		setImplementation(cont);
		m_id = script["name"].as< std::string >();	// TODO: separate id element in schema
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
*/
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


	prm::param_wgt* create_widget_from_schema(
//		std::string const& schema_name,	// This is the value of the 'name' attribute, for which the following schema node is the value of the 'children' attribute 
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

			std::string name = e["name"].as< std::string >();
			prm::ParamType type = e["type"].as< prm::ParamType >();
			YAML::Node constraints;
			if(e["constraints"])
			{
				constraints = e["constraints"];
			}

			if(type == prm::ParamType::List)
			{
				assert(e["children"]);
				YAML::Node const& sub = e["children"];
				assert(sub.IsSequence());

				parent->add_child(create_widget_from_schema(//e["name"].Scalar(), sub,
					e,
					schema_accessor));
			}
			else
			{
				prm::param_wgt* w = prm::param_wgt::create(e);//type, parse_constraints(constraints, type), name /* todo: separate id */, name);

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
//		std::string const& schema_name,	// This is the value of the 'name' attribute, for which the following schema node is the value of the 'children' attribute 
		YAML::Node const& schema,
		std::shared_ptr< std::function< YAML::Node(std::string) > > const& schema_accessor)
	{
		//assert(schema.IsSequence());
		assert(schema.IsMap() && schema["type"] && schema["type"].as< prm::ParamType >() == prm::ParamType::List);

		// TODO: Group box/label??
/*		prm::container_par_wgt* wgt = (prm::container_par_wgt*)prm::param_wgt::create(prm::ParamType::List, prm::par_null_constraints(), schema_name);

		create_widget_contents_from_schema(wgt, schema_name, schema, schema_accessor);
*/

		prm::container_par_wgt* wgt = (prm::container_par_wgt*)prm::param_wgt::create(schema);
		create_widget_contents_from_schema(wgt, schema["name"].as< std::string >(), schema["children"], schema_accessor);

		return wgt;
	}


	YAML::Node find_subschema(YAML::Node const& schema, std::string const& name)
	{
		for(auto const& e : schema)
		{
			if(e["type"].as< prm::ParamType >() != prm::ParamType::List)
			{
				continue;
			}

			if(e["name"].Scalar() == name)
			{
				return e["children"];
			}
			else if(YAML::Node n = find_subschema(e["children"], name))
			{
				return n;
			}
		}

		return YAML::Node();
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

		param_wgt* wgt = create_widget_from_schema(/*"root",*/ hack/*schema*/, schema_accessor);

		*schema_accessor = [wgt, schema_provider](std::string name)
		{
			YAML::Node param_vals = wgt->get_yaml_param();
			YAML::Node complete_schema = schema_provider(param_vals);
			return name == "root" ? complete_schema : find_subschema(complete_schema, name);
		};

		return wgt;
	}


	void param_wgt::connect_changed_handler(boost::function< void() > const& handler)
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

	YAML::Node param_wgt::get_yaml_param() const
	{
		return boost::apply_visitor(yaml_param_visitor(), get_param());
	}
}




