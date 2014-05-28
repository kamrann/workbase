
#include "params.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WContainerWidget>
#include <Wt/WTime>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>

#include <string>
#include <sstream>
#include <iostream>

using namespace Wt;
using namespace std;
namespace dbo = Wt::Dbo;


dbo::backend::Sqlite3 the_db("test.db");


class TestApp: public WApplication
{
public:
	TestApp(const WEnvironment& env): WApplication(env)
	{
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
	}

	void on_add()
	{
		dbo::Transaction t(session);
		db_hierarchy_level< Animal >::add_record_from_input(session, m_mgr);
		t.commit();
	}

	prm::param_mgr m_mgr;
	dbo::Session session;
};


WApplication* createApplication(const WEnvironment& env)
{
	return new TestApp(env);
}

int main(int argc, char* argv[])
{
	return WRun(argc, argv, &createApplication);
}

