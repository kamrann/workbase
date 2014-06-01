
#include "yaml_test.h"

//#include "params.h"
#include "params/param.h"

#include "wt_param_widgets/par_wgt.h"
#include "wt_param_widgets/container_par_wgt.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WContainerWidget>
#include <Wt/WTime>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>

#include <yaml-cpp/yaml.h>

#include <string>
#include <sstream>
#include <iostream>
#include <map>

using namespace Wt;
using namespace std;
namespace dbo = Wt::Dbo;


namespace YAML {
	template <>
	struct convert< prm::ParamType >
	{
		static Node encode(prm::ParamType const& rhs)
		{
			return Node(names_[rhs]);
		}

		static bool decode(Node const& node, prm::ParamType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::string const names_[prm::ParamType::Count];
		static std::map< std::string, prm::ParamType > const mapping_;
	};

	std::string const convert< prm::ParamType >::names_[prm::ParamType::Count] = {
		"bool",
		"int",
		"real",
		"string",
		"enum",
		"vector2",
		//"vector3",
		"container",
	};

	std::map< std::string, prm::ParamType > const convert< prm::ParamType >::mapping_ = {
		{ "bool", prm::ParamType::Boolean },
		{ "int", prm::ParamType::Integer },
		{ "real", prm::ParamType::RealNumber },
		{ "string", prm::ParamType::String },
		{ "enum", prm::ParamType::Enumeration },
		{ "vector2", prm::ParamType::Vector2 },
		//{ "vector3", prm::ParamType::Vector3 },
		{ "container", prm::ParamType::List },
	};
}


dbo::backend::Sqlite3 the_db("test.db");



YAML::Node yaml_spec_A::get_param_schema(YAML::Node const& param_vals)
{
	YAML::Node s, n, constraints;

	n["name"] = "A Param 1 is a real number";
	n["type"] = prm::ParamType::RealNumber;//"real";
	s.push_back(n);
	
	n.reset();
	n["name"] = "A Param 2 is a boolean";
	n["type"] = prm::ParamType::Boolean;//"bool";
	s.push_back(n);

	return s;
}

YAML::Node yaml_spec_B::get_param_schema(YAML::Node const& param_vals)
{
	YAML::Node s, n, constraints;

	n["name"] = "B Param 1 is an integer";
	n["type"] = prm::ParamType::Integer;//"int";
	//	ss << " constraints: <>\n";
	s.push_back(n);

	n.reset();
	n["name"] = "B Param 2 is an enum";
	n["type"] = prm::ParamType::Enumeration;
	constraints.reset();
	constraints["values"].push_back("Nested A");
	constraints["values"].push_back("Another integer");
	n["constraints"] = constraints;
	n["update"] = "default";
	s.push_back(n);

	n.reset();
	std::string p2enum_sel = param_vals["B Param 2 is an enum"].as< std::string >();	// For now assuming all enum values are strings
	if(p2enum_sel == "Nested A")
	{
		n["name"] = "<?Currently unused?>";
		n["type"] = prm::ParamType::List;
		n["children"] = yaml_spec_A().get_param_schema();
	}
	else if(p2enum_sel == "Another integer")
	{
		n["name"] = "Here is another integer";
		n["type"] = prm::ParamType::Integer;
	}
	else
	{
		assert(false);
	}
	s.push_back(n);

	return s;
}

prm::par_constraints parse_constraints(YAML::Node const& constraints, prm::ParamType type)
{
	assert(constraints.IsMap() || constraints.IsNull());
	switch(type)
	{
		case prm::Boolean:
		return prm::boolean_par_constraints();

		case prm::Integer:
		return prm::integer_par_constraints();

		case prm::RealNumber:
		return prm::realnum_par_constraints();

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


void create_widget_contents_from_schema(
	prm::container_par_wgt* parent,
	std::string const& name,
	YAML::Node const& schema,
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

			parent->add_child(create_widget_from_schema(e["name"].Scalar(), sub));
		}
		else
		{
			prm::param_wgt* w = prm::param_wgt::create(type, parse_constraints(constraints, type), name);
			if(e["update"] && e["update"].Scalar() == "default")
			{
				w->connect_changed_handler([name, parent, i]
				{
					YAML::Node sub_schema = get_schema_from(
						name, /* todo: should be id? */
						param_mgr->get_root_param() -> encapsulate in get_schema_from functor
						);

					parent->clear_children(i + 1);
					create_widget_contents_from_schema(parent, name, sub_schema, i + 1);
				});
			}
			parent->add_child(w);
		}
	}
}

prm::param_wgt* create_widget_from_schema(
	std::string const& name,	// This is the value of the 'name' attribute, for which the following scheme node is the value of the 'children' attribute 
	YAML::Node const& schema)
{
	assert(schema.IsMap());
	assert(schema["type"] && schema["type"].as< prm::ParamType >() == prm::ParamType::List);

	// TODO: Group box/label??
	prm::container_par_wgt* wgt = (prm::container_par_wgt*)prm::param_wgt::create(prm::ParamType::List, prm::par_null_constraints());

	create_widget_contents_from_schema(wgt, schema["name"].Scalar(), schema["children"]);

	return wgt;
}


class TestApp: public WApplication
{
public:
	TestApp(const WEnvironment& env): WApplication(env)
	{
/*
		Wt::WWidget* w = db_hierarchy_level< Animal >::create_widget(m_mgr);
		root()->addWidget(w);

		Wt::WPushButton* btn = new Wt::WPushButton("Add");
		btn->clicked().connect(this, &TestApp::on_add);
		root()->addWidget(btn);

		session.setConnection(the_db);
		db_hierarchy_level< Animal >::map_database_table(session);
		db_hierarchy_level< House >::map_database_table(session);
		db_hierarchy_level< Dog >::map_database_table(session);
		db_hierarchy_level< Cat >::map_database_table(session);
		db_hierarchy_level< DomesticCat >::map_database_table(session);
		db_hierarchy_level< Tiger >::map_database_table(session);

		try
		{
			dbo::Transaction t(session);
			session.createTables();
			t.commit();

			std::cerr << "Created database" << std::endl;
		}
		catch(std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			std::cerr << "Using existing database";
		}
*/

		yaml_spec_B specB;
		
		YAML::Node schema = specB.get_param_schema();
		assert(!schema.IsNull());
		assert(schema.IsDefined());
		assert(schema.IsSequence());

		prm::param_wgt* wgt = create_widget_from_schema(schema);
		root()->addWidget(wgt);
	}

/*	void on_add()
	{
		dbo::Transaction t(session);
		db_hierarchy_level< Animal >::add_record_from_input(session, m_mgr);
		t.commit();
	}
*/
	//prm::param_mgr m_mgr;
	//dbo::Session session;
};


WApplication* createApplication(const WEnvironment& env)
{
	return new TestApp(env);
}

int main(int argc, char* argv[])
{
	return WRun(argc, argv, &createApplication);
}

