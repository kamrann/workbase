// webinterfaceapp.cpp

#include "webinterfaceapp.h"
//#include "database_tab.h"
#include "simulations_tab.h"
#include "system_test_tab.h"

// TODO: temp
#include "wt_system_widgets/nac_widget.h"

#include <Wt/WTabWidget>
#include <Wt/WServer>

#include <Wt/WBootstrapTheme>
#include <Wt/WCssTheme>


WebInterfaceApplication::WebInterfaceApplication(const WEnvironment& env, dbo::SqlConnectionPool& db):
	WApplication(env),
	db_cp(&db),
	db_session(db)
{
	setTitle("NeuroEvo Web Interface");

	bool const BootstrapTheme = false;

	if(BootstrapTheme)
	{
		auto theme = new Wt::WBootstrapTheme(this);
		theme->setVersion(Wt::WBootstrapTheme::Version3);
		theme->setResponsive(true);
		setTheme(theme);

		// load the default bootstrap3 (sub-)theme
		useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");
	}
	else
	{
		setCssTheme("polished");
	}

	styleSheet().addRule(".wb_numerical_input", "text-align: right;");

	main_tabs = new WTabWidget(root());
//	main_tabs->addTab(new DatabaseTab(db_session), "Evo Database");// , Wt::WTabWidget::PreLoading);
	main_tabs->addTab(new Wt::WContainerWidget(), "Placeholder");
	main_tabs->addTab(new SimulationsTab(), "Simulations");// , Wt::WTabWidget::PreLoading);
	SystemTestTab* sys_tab = new SystemTestTab();
	main_tabs->addTab(sys_tab, "System Test");// , Wt::WTabWidget::PreLoading);
	main_tabs->setStyleClass("tabwidget");

//	noughts_and_crosses_widget< 2, 3 >* nac_w = new noughts_and_crosses_widget< 2, 3 >();
//	sys_tab->set_system_widget(nac_w);

	useStyleSheet("style/param_widgets.css");

/*	useStyleSheet("style/everywidget.css");
	useStyleSheet("style/dragdrop.css");
	useStyleSheet("style/combostyle.css");
	useStyleSheet("style/pygments.css");
	useStyleSheet("style/layout.css");
*/
	enableUpdates(true);	// TODO: Right place?
}

WApplication* createApplication(const WEnvironment& env, dbo::SqlConnectionPool* db)
{
	/*
	* You could read information from the environment to decide whether
	* the user has permission to start a new application
	*/
	return new WebInterfaceApplication(env, *db);
}

int main(int argc, char **argv)
{
	/*
	* Your main method may set up some shared resources, but should then
	* start the server application (FastCGI or httpd) that starts listening
	* for requests, and handles all of the application life cycles.
	*
	* The last argument to WRun specifies the function that will instantiate
	* new application objects. That function is executed when a new user surfs
	* to the Wt application, and after the library has negotiated browser
	* support. The function should return a newly instantiated application
	* object.
	*/

	WServer server(argv[0]);
	server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

	Wt::Dbo::SqlConnectionPool* db = evodb_session::create_connection_pool(server.appRoot() + "evo.db");

	server.addEntryPoint(Application, boost::bind(&createApplication, _1, db), "/home");

	if(server.start())
	{
		WServer::waitForShutdown();
		server.stop();
	}

	return 0;

//	return WRun(argc, argv, &createApplication);
}
