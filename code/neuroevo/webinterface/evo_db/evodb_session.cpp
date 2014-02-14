// evodb_session.cpp

#include "evodb_session.h"
#include "system_sim_tbl.h"
#include "problem_domain_tbl.h"
#include "genetic_language_tbl.h"
#include "evo_period_tbl.h"
#include "generation_tbl.h"
#include "genome_tbl.h"

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

	mapClass< system_sim >("system_sim");
	mapClass< problem_domain >("problem_domain");
	mapClass< genetic_language >("genetic_language");
	mapClass< evo_period >("evo_period");
	mapClass< generation >("generation");
	mapClass< genome >("genome");

	try
	{
		dbo::Transaction t(*this);

		createTables();

		t.commit();

		std::cerr << "Created database" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Using existing database";
	}
}

