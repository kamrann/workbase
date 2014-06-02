
#include "yaml_test.h"

//#include "params.h"
#include "params/param.h"

#include "wt_param_widgets/par_wgt.h"
#include "wt_param_widgets/container_par_wgt.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WTextArea>
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



dbo::backend::Sqlite3 the_db("test.db");



YAML::Node yaml_spec_A::get_param_schema(YAML::Node const& param_vals)
{
	prm::schema_builder sb;

	sb.add_real(
		"A Param 1 is a real number",
		-1.5
		);
	
	sb.add_boolean(
		"A Param 2 is a boolean",
		true
		);

	return sb.get_schema();
}

YAML::Node yaml_spec_B::get_param_schema(YAML::Node const& param_vals)
{
	prm::schema_builder sb;

	sb.add_integer(
		"B Param 1 is an integer",
		1,
		1,
		10
		);

	sb.add_enum_selection(
		"B Param 2 is an enum",
		{ "Nested A", "Another integer" }
		);
	sb.on_update();

	auto node = param_vals["B Param 2 is an enum"];
	std::string p2enum_sel = node ? node.as< std::string >() : "Nested A";	// For now assuming all enum values are strings
	if(p2enum_sel == "Nested A")
	{
		sb.add_nested_schema(
			"Nested name",
			yaml_spec_A().get_param_schema(param_vals["Nested name"])
			);
	}
	else if(p2enum_sel == "Another integer")
	{
		sb.add_integer(
			"Here is another integer",
			0,
			-1,
			1
			);
	}
	else
	{
		assert(false);
	}

	return sb.get_schema();
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

/*		yaml_spec_B specB;
		YAML::Node schema = specB.get_param_schema(YAML::Node());
		prm::param_wgt* wgt = create_widget_from_schema("root", schema, schema_accessor);
		root()->addWidget(wgt);

		schema_accessor = [wgt](std::string name)
		{
			YAML::Node param_vals = wgt->get_yaml_param();
			YAML::Node complete_schema = yaml_spec_B().get_param_schema(param_vals);
			return name == "root" ? complete_schema : find_subschema(complete_schema, name);
		};
*/
		prm::param_wgt* wgt = prm::param_wgt::create([](YAML::Node const& param_vals)
		{
			return yaml_spec_B().get_param_schema(param_vals);
		});
		root()->addWidget(wgt);

		Wt::WPushButton* btn = new Wt::WPushButton("Show YAML");
		btn->setInline(false);
		root()->addWidget(btn);

		Wt::WTextArea* text = new Wt::WTextArea();
		text->resize(500, 400);
		root()->addWidget(text);

		btn->clicked().connect(std::bind([text, wgt]
		{
			YAML::Node vals = wgt->get_yaml_param();
			text->setText(YAML::Dump(vals));
		}));
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

	//std::function< YAML::Node(std::string) > schema_accessor;
};


WApplication* createApplication(const WEnvironment& env)
{
	return new TestApp(env);
}

int main(int argc, char* argv[])
{
	return WRun(argc, argv, &createApplication);
}

