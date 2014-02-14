// database_tab.cpp

#include "database_tab.h"
#include "evo_db/evodb_session.h"
#include "evo_db/system_sim_tbl.h"
#include "evo_db/problem_domain_tbl.h"
#include "evo_db/genetic_language_tbl.h"
#include "evo_db/evo_period_tbl.h"
#include "evo_db/generation_tbl.h"
#include "evo_db/genome_tbl.h"

#include <Wt/WTableView>
#include <Wt/WPanel>
#include <Wt/WItemDelegate>
#include <Wt/WApplication>

#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>


DatabaseTab::DatabaseTab(evodb_session& dbs, WContainerWidget* parent):
	WContainerWidget(parent),
	db_session(dbs)
{
	dbo::Query< dbo::ptr< evo_period > > period_query = db_session.find< evo_period >();
	dbo::QueryModel< dbo::ptr< evo_period > >* period_q_model = new dbo::QueryModel< dbo::ptr< evo_period > >(this);
	period_q_model->setQuery(period_query);
	{
		auto const& fields = period_q_model->fields();
		for(size_t f = 0; f < fields.size(); ++f)
		{
			if(!fields[f].isIdField() && !fields[f].isVersionField() && !fields[f].isForeignKey())
			{
				period_q_model->addColumn(fields[f].name(), fields[f].name());
			}
		}
	}

	dbo::Query< dbo::ptr< generation > > generation_query = db_session.find< generation >().where("gen_period_id = ?").bind("0");
	dbo::QueryModel< dbo::ptr< generation > >* generation_q_model = new dbo::QueryModel< dbo::ptr< generation > >(this);
	generation_q_model->setQuery(generation_query);
	{
		auto const& fields = generation_q_model->fields();
		for(size_t f = 0; f < fields.size(); ++f)
		{
			if(!fields[f].isIdField() && !fields[f].isVersionField() && !fields[f].isForeignKey())
			{
				generation_q_model->addColumn(fields[f].name(), fields[f].name());
			}
		}
	}

	dbo::Query< dbo::ptr< genome > > individual_query = db_session.find< genome >().where("geno_gen_id = ?").bind("0");
	dbo::QueryModel< dbo::ptr< genome > >* individual_q_model = new dbo::QueryModel< dbo::ptr< genome > >(this);
	individual_q_model->setQuery(individual_query);
	{
		auto const& fields = individual_q_model->fields();
		for(size_t f = 0; f < fields.size(); ++f)
		{
			if(!fields[f].isIdField() && !fields[f].isVersionField() && !fields[f].isForeignKey())
			{
				individual_q_model->addColumn(fields[f].name(), fields[f].name());
			}
		}
	}

	WPanel* panel = new WPanel(this);
	panel->setTitle("Evolution Periods");

	period_table = new WTableView();
	period_table->setMargin(10, Top | Bottom);
	period_table->setMargin(WLength::Auto, Left | Right);
	period_table->setModel(period_q_model);
	period_table->setSortingEnabled(false);
	period_table->setMaximumSize(WLength::Auto, 300);
	period_table->setSelectionMode(Wt::SingleSelection);
	//	db_table->setHeaderHeight(30);
	//	db_table->setColumnAlignment(0, AlignCenter);
	//	db_table->setHeaderAlignment(0, AlignCenter);
	//	db_table->setColumnWidth(1, 100);
	//	db_table->setRowHeight(22);

	panel->setCentralWidget(period_table);

	period_table->selectionChanged().connect(this, &DatabaseTab::on_period_sel_changed);

	panel = new WPanel(this);
	panel->setTitle("Generations");

	generation_table = new WTableView();
	generation_table->setMargin(10, Top | Bottom);
	generation_table->setMargin(WLength::Auto, Left | Right);
	generation_table->setModel(generation_q_model);
	generation_table->setSortingEnabled(false);
	generation_table->setMaximumSize(WLength::Auto, 300);
	generation_table->setSelectionMode(Wt::SingleSelection);

	WItemDelegate* delegate = new WItemDelegate(this);
	delegate->setTextFormat("%.2f");
	generation_table->setItemDelegateForColumn(1, delegate);

	panel->setCentralWidget(generation_table);

	generation_table->selectionChanged().connect(this, &DatabaseTab::on_generation_sel_changed);

	panel = new WPanel(this);
	panel->setTitle("Individuals");

	individual_table = new WTableView();
	individual_table->setMargin(10, Top | Bottom);
	individual_table->setMargin(WLength::Auto, Left | Right);
	individual_table->setModel(individual_q_model);
	individual_table->setSortingEnabled(false);
	individual_table->setMaximumSize(WLength::Auto, 300);
	individual_table->setSelectionMode(Wt::SingleSelection);

	panel->setCentralWidget(individual_table);

	individual_table->doubleClicked().connect(this, &DatabaseTab::on_individual_dbl_clicked);

	db_session.new_evo_period_signal().connect(this, &DatabaseTab::on_new_evo_period);
	db_session.new_generations_signal().connect(this, &DatabaseTab::on_new_generations);
}

void DatabaseTab::on_period_sel_changed()
{
	dbo::Query< dbo::ptr< generation > > generation_query = db_session.find< generation >().where("gen_period_id = ?");
	auto sel = period_table->selectedIndexes();
	if(!sel.empty())
	{
		dbo::QueryModel< dbo::ptr< evo_period > >* period_mod = (dbo::QueryModel< dbo::ptr< evo_period > >*)period_table->model();
		std::stringstream ss;
		ss << period_mod->resultRow(sel.begin()->row()).id();
		generation_query.bind(ss.str());
		dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
		generation_mod->setQuery(generation_query, true);
		//		generation_mod->reload();
	}
}

void DatabaseTab::on_generation_sel_changed()
{
	dbo::Query< dbo::ptr< genome > > individual_query = db_session.find< genome >().where("geno_gen_id = ?");
	auto sel = generation_table->selectedIndexes();
	if(!sel.empty())
	{
		dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
		std::stringstream ss;
		ss << generation_mod->resultRow(sel.begin()->row()).id();
		individual_query.bind(ss.str());
		dbo::QueryModel< dbo::ptr< genome > >* individual_mod = (dbo::QueryModel< dbo::ptr< genome > >*)individual_table->model();
		individual_mod->setQuery(individual_query, true);
		//		generation_mod->reload();
	}
}

void DatabaseTab::on_individual_dbl_clicked()
{
/*	dbo::ptr< genome > > generation_query = db_session.find< generation >().where("gen_period_id = ?");
	auto sel = period_table->selectedIndexes();
	if(!sel.empty())
	{
		dbo::QueryModel< dbo::ptr< evo_period > >* period_mod = (dbo::QueryModel< dbo::ptr< evo_period > >*)period_table->model();
		std::stringstream ss;
		ss << period_mod->resultRow(sel.begin()->row()).id();
		generation_query.bind(ss.str());
		dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
		generation_mod->setQuery(generation_query, true);
		//		generation_mod->reload();
	}
	*/
}

void DatabaseTab::on_new_evo_period(dbo::ptr< evo_period > ep)
{
	dbo::QueryModel< dbo::ptr< evo_period > >* period_mod = (dbo::QueryModel< dbo::ptr< evo_period > >*)period_table->model();
	period_mod->reload();
	WApplication::instance()->triggerUpdate();
}

void DatabaseTab::on_new_generations()
{
	dbo::QueryModel< dbo::ptr< generation > >* generation_mod = (dbo::QueryModel< dbo::ptr< generation > >*)generation_table->model();
	generation_mod->reload();
	WApplication::instance()->triggerUpdate();
}

