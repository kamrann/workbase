// evodb_session.cpp

#include "evodb_session.h"

#include "evo_run_tbl.h"
#include "generation_tbl.h"
#include "genome_tbl.h"
#include "named_genome_tbl.h"

#include <Wt/Dbo/FixedSqlConnectionPool>
//#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/backend/Postgres>


dbo::SqlConnectionPool* evodb_session::create_connection_pool(const std::string& conn_str)
{
	try
	{
		auto connection =
			//new dbo::backend::Sqlite3(conn_str);
			new dbo::backend::Postgres(conn_str);

		connection->setProperty("show-queries", "false");
		//	connection->setDateTimeStorage(Wt::Dbo::SqlDateTime, Wt::Dbo::backend::Sqlite3::PseudoISO8601AsText);

		return new dbo::FixedSqlConnectionPool(connection, 10);	// NOTE: If use value of 1, all transactions will be serialized
	}
	catch(Wt::WException& e)
	{
		std::cout << "DB Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

evodb_session::evodb_session(dbo::SqlConnectionPool& conn_pool): connection_pool(conn_pool)
{
	setConnectionPool(connection_pool);

	mapClass< evo_run >("evo_run");
	mapClass< generation >("generation");
	mapClass< genome >("genome");
	mapClass< named_genome >("named_genome");

	try
	{
		dbo::Transaction t(*this);
		createTables();
//		check_static_tables();
		t.commit();

		std::cerr << "Created database" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Using existing database" << std::endl;
	}
}
/*
void evodb_session::check_static_tables()
{
	// TODO: This should be automated through parameter hierarchy definition

	// TODO: May actually be better to not tie in the enum values to the database, but use string names to identify system types
	// (and any other enum types), since names can easily remain constant, but removing a no longer wanted enum entry will fuck
	// up values - UNLESS we manually assign value to each enum value.
	for(size_t i = 0; i < SystemType::NumSystems; ++i)
	{
		SystemType st = (SystemType)i;
		dbo::ptr< sys_type > t;
		t = find< sys_type >().where("type == ?").bind(st);
		if(!t)
		{
			t = add(new sys_type);
			t.modify()->type = st;
			t.modify()->name = SystemNames[i];
		}
	}

	for(size_t i = 0; i < rtp::agent_body_spec::Type::Count; ++i)
	{
		rtp::agent_body_spec::Type at = (rtp::agent_body_spec::Type)i;
		dbo::ptr< phys_agent_type > t;
		t = find< phys_agent_type >().where("type == ?").bind(at);
		if(!t)
		{
			t = add(new phys_agent_type);
			t.modify()->type = at;
			t.modify()->name = rtp::agent_body_spec::Names[i];
		}
	}

	for(size_t i = 0; i < rtp::evolvable_controller::Type::Count; ++i)
	{
		rtp::evolvable_controller::Type ct = (rtp::evolvable_controller::Type)i;
		dbo::ptr< phys_controller_type > t;
		t = find< phys_controller_type >().where("type == ?").bind(ct);
		if(!t)
		{
			t = add(new phys_controller_type);
			t.modify()->type = ct;
			t.modify()->name = rtp::evolvable_controller::Names[i];
		}
	}

	for(size_t i = 0; i < rtp::mlp_controller::Type::Count; ++i)
	{
		rtp::mlp_controller::Type ct = (rtp::mlp_controller::Type)i;
		dbo::ptr< phys_mlp_inputs_type > t;
		t = find< phys_mlp_inputs_type >().where("type == ?").bind(ct);
		if(!t)
		{
			t = add(new phys_mlp_inputs_type);
			t.modify()->type = ct;
			t.modify()->name = rtp::mlp_controller::Names[i];
		}
	}
}
*/

