// evodb_session.cpp

#include "evodb_session.h"
/*
//#include "system_sim_tbl.h"
//#include "problem_domain_tbl.h"
//#include "genetic_language_tbl.h"
#include "evo_run_tbl.h"
#include "sys_params_tbl.h"
#include "sys_type_tbl.h"
#include "phys_sys_params_tbl.h"
#include "phys_agent_params_tbl.h"
#include "phys_agent_type_tbl.h"
#include "phys_controller_params_tbl.h"
#include "phys_controller_type_tbl.h"
#include "phys_mlp_controller_params_tbl.h"
#include "phys_mlp_inputs_type_tbl.h"
#include "nac_sys_params_tbl.h"
#include "evo_params_tbl.h"
#include "generation_tbl.h"
#include "genome_tbl.h"
*/

#include "../evorun_params.h"
#include "../system_params.h"
#include "../phys_system_params.h"
#include "../phys_agent_params.h"
#include "../phys_controller_params.h"
#include "../genalg_params.h"
#include "../generation_params.h"
#include "../genome_params.h"

#include <Wt/Dbo/FixedSqlConnectionPool>


dbo::SqlConnectionPool* evodb_session::create_connection_pool(const std::string& sqlite3_db)
{
	dbo::backend::Sqlite3* connection = new dbo::backend::Sqlite3(sqlite3_db);

	connection->setProperty("show-queries", "false");
	connection->setDateTimeStorage(Wt::Dbo::SqlDateTime, Wt::Dbo::backend::Sqlite3::PseudoISO8601AsText);

	return new dbo::FixedSqlConnectionPool(connection, 10);	// NOTE: If use value of 1, all transactions will be serialized
}

evodb_session::evodb_session(dbo::SqlConnectionPool& conn_pool): connection_pool(conn_pool)
{
	setConnectionPool(connection_pool);

/*
	mapClass< evo_run >("evo_run");
	mapClass< sys_params >("sys_params");
	mapClass< sys_type >("sys_type");
	mapClass< phys_sys_params >("phys_sys_params");
	mapClass< phys_agent_params >("phys_agent_params");
	mapClass< phys_agent_type >("phys_agent_type");
	mapClass< phys_controller_params >("phys_controller_params");
	mapClass< phys_controller_type >("phys_controller_type");
	mapClass< phys_mlp_controller_params >("phys_mlp_controller_params");
	mapClass< phys_mlp_inputs_type >("phys_mlp_inputs_type");
	mapClass< nac_sys_params >("nac_sys_params");
	mapClass< evo_params >("evo_params");
	mapClass< generation >("generation");
	mapClass< genome >("genome");
*/

	db_hierarchy_level< evorun_params_defn >::map_database_table(*this);
	db_hierarchy_level< system_params_defn >::map_database_table(*this);
	db_hierarchy_level< phys::phys_system_params_defn >::map_database_table(*this);
	db_hierarchy_level< phys::phys_agent_params_defn >::map_database_table(*this);
	db_hierarchy_level< phys::phys_controller_params_defn >::map_database_table(*this);
	db_hierarchy_level< genalg_params_defn >::map_database_table(*this);
	db_hierarchy_level< generation_params_defn >::map_database_table(*this);
	db_hierarchy_level< genome_params_defn >::map_database_table(*this);

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
		std::cerr << "Using existing database";
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

	for(size_t i = 0; i < rtp_phys::agent_body_spec::Type::Count; ++i)
	{
		rtp_phys::agent_body_spec::Type at = (rtp_phys::agent_body_spec::Type)i;
		dbo::ptr< phys_agent_type > t;
		t = find< phys_agent_type >().where("type == ?").bind(at);
		if(!t)
		{
			t = add(new phys_agent_type);
			t.modify()->type = at;
			t.modify()->name = rtp_phys::agent_body_spec::Names[i];
		}
	}

	for(size_t i = 0; i < rtp_phys::evolvable_controller::Type::Count; ++i)
	{
		rtp_phys::evolvable_controller::Type ct = (rtp_phys::evolvable_controller::Type)i;
		dbo::ptr< phys_controller_type > t;
		t = find< phys_controller_type >().where("type == ?").bind(ct);
		if(!t)
		{
			t = add(new phys_controller_type);
			t.modify()->type = ct;
			t.modify()->name = rtp_phys::evolvable_controller::Names[i];
		}
	}

	for(size_t i = 0; i < rtp_phys::mlp_controller::Type::Count; ++i)
	{
		rtp_phys::mlp_controller::Type ct = (rtp_phys::mlp_controller::Type)i;
		dbo::ptr< phys_mlp_inputs_type > t;
		t = find< phys_mlp_inputs_type >().where("type == ?").bind(ct);
		if(!t)
		{
			t = add(new phys_mlp_inputs_type);
			t.modify()->type = ct;
			t.modify()->name = rtp_phys::mlp_controller::Names[i];
		}
	}
}
*/

