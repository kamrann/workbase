// webinterfaceapp.h

#ifndef __WEB_INTERFACE_APP_H
#define __WEB_INTERFACE_APP_H

#include "evo_db/evodb_session.h"

#include <Wt/WApplication>


using namespace Wt;

class Wt::WTabWidget;

class WebInterfaceApplication : public WApplication
{
public:
	WebInterfaceApplication(const WEnvironment& env, dbo::SqlConnectionPool& db);

private:
	void run_alg();
	void period_sel_changed();

public:
	dbo::SqlConnectionPool* db_cp;
	evodb_session db_session;

	WTabWidget* main_tabs;
};


#endif


