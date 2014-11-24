// evodb_session..h

#ifndef __EVO_DB_SESSION_H
#define __EVO_DB_SESSION_H

#include "table_traits.h"

#include <Wt/WSignal>
#include <Wt/Auth/Login>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>


namespace dbo = Wt::Dbo;

class evodb_session: public dbo::Session
{
public:
	evodb_session(dbo::SqlConnectionPool& conn_pool);

public:
	// TODO: Don't like this setup, since we can have multiple sessions on the same DB, but signalling will only be to
	// events registered with the particular session object for which signal is emitted...
/*	Wt::Signal< dbo::ptr< evo_run > >& new_evo_period_signal()
	{
		return new_evo_period_sig;
	}

	Wt::Signal<>& new_generations_signal()
	{
		return new_gen_sig;
	}
*/
	static dbo::SqlConnectionPool* create_connection_pool(const std::string& conn_str);

private:
//	void check_static_tables();

private:
	dbo::SqlConnectionPool& connection_pool;

//	Wt::Signal< dbo::ptr< evo_run > > new_evo_period_sig;
//	Wt::Signal<> new_gen_sig;
};


#endif

