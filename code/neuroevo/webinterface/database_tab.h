// database_tab.h

#ifndef __DATABASE_TAB_H
#define __DATABASE_TAB_H

#include <Wt/WContainerWidget>


using namespace Wt;
namespace dbo = Wt::Dbo;

class Wt::WTableView;

class evodb_session;
class evo_run;
class generation;

class DatabaseTab: public WContainerWidget
{
public:
	DatabaseTab(evodb_session& dbs, WContainerWidget* parent = nullptr);

private:
	evodb_session& db_session;

	WTableView* period_table;
	WTableView* generation_table;
	WTableView* individual_table;

public:
	void on_period_sel_changed();
	void on_generation_sel_changed();
	void on_individual_dbl_clicked();
	void on_new_evo_period(dbo::ptr<  > ep);
		//dbo::ptr< evo_run > ep);
	void on_new_generations();
};


#endif

