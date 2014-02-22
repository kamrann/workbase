// ship_history_widget.h

#ifndef __SHIP_AND_THRUSTERS_HISTORY_WIDGET_H
#define __SHIP_AND_THRUSTERS_HISTORY_WIDGET_H

/*
#include "systems/ship_and_thrusters/ship_and_thrusters_system.h"
// TEMP
#include "systems/ship_and_thrusters/scenarios/full_stop.h"
//
*/
#include "../rtp_interface/systems/sat/rtp_sat_system.h"


#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WApplication>
#include <Wt/WTable>
#include <Wt/WCheckBox>

#include <boost/math/constants/constants.hpp>


template <
	WorldDimensionality Dim_
>
class ship_history_widget;

// 2D Widget specialization
template <>
class ship_history_widget< WorldDimensionality::dim2D >: public Wt::WContainerWidget
{
public:
	static WorldDimensionality const Dim = WorldDimensionality::dim2D;

	enum {
		Timestep = 0,
		PosX,
		PosY,
		Angle,
		VelX,
		VelY,
		AngVel,
		Speed,
		AngSpeed,
		ForceX,
		ForceY,
		Force,
		Torque,
		Kinetic,
		AvgKinetic,

		NumColumns,
	};

	struct series_cfg
	{
		Wt::WCheckBox* enabled_box;
	};

//	typedef ship_and_thrusters_system< Dim >	system_t;
//	typedef full_stop< Dim >::state				state_t;
	typedef sat_system< Dim >::state			state_t;

public:
	ship_history_widget(): model(nullptr)
	{
//		setLayoutSizeAware(true);

		Wt::WHBoxLayout* layout = new Wt::WHBoxLayout;
		setLayout(layout);

		model = new Wt::WStandardItemModel();
		model->insertColumns(0, NumColumns);
		model->setHeaderData(Timestep, "Timestep");
		model->setHeaderData(PosX, "Pox X");
		model->setHeaderData(PosY, "Pox Y");
		model->setHeaderData(Angle, "Orientation");
		model->setHeaderData(VelX, "Vel X");
		model->setHeaderData(VelY, "Vel Y");
		model->setHeaderData(AngVel, "Angular Vel");
		model->setHeaderData(Speed, "Speed");
		model->setHeaderData(AngSpeed, "Angular Speed");
		model->setHeaderData(ForceX, "Force X");
		model->setHeaderData(ForceY, "Force Y");
		model->setHeaderData(Force, "Force");
		model->setHeaderData(Torque, "Torque");
		model->setHeaderData(Kinetic, "KE");
		model->setHeaderData(AvgKinetic, "Avg(KE)");

		chart = new Wt::Chart::WCartesianChart(this);
		chart->setBackground(Wt::WColor(220, 220, 220));
		chart->setModel(model);
		chart->setXSeriesColumn(Timestep);
		chart->setLegendEnabled(true);
		chart->setType(Wt::Chart::ScatterPlot);
//		chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);

		/*
		* Provide ample space for the title, the X and Y axis and the legend.
		*/
		chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
		chart->setPlotAreaPadding(120, Wt::Right);

		//chart->addSeries(Wt::Chart::WDataSeries(Kinetic, Wt::Chart::LineSeries));
		//chart->addSeries(Wt::Chart::WDataSeries(AvgKinetic, Wt::Chart::LineSeries));

		chart->resize(500, 400);
		chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);

		layout->addWidget(chart, 1);

		cfg_table = new WTable(this);
		cfg_table->setMargin(WLength::Auto, Left | Right);
		for(int i = 1; i < model->columnCount(); ++i)
		{
			new WText(asString(model->headerData(i)), cfg_table->elementAt(i - 1, 0));

			series_cfg& sc = series_config[i];
			sc.enabled_box = new WCheckBox(cfg_table->elementAt(i - 1, 1));
			sc.enabled_box->changed().connect(this, &ship_history_widget::on_config_changed);
		}

		layout->addWidget(cfg_table, 0, Wt::AlignMiddle);

		reset();
	}

public:
	void reset()
	{
		model->removeRows(0, model->rowCount());
		sum_kinetic = 0.0;
	}
	
	void update_from_system_state(state_t* st)
	{
		model->appendRow(create_data_row(st));
		chart->update();
		WApplication::instance()->triggerUpdate();
	}

protected:
	std::vector< Wt::WStandardItem* > create_data_row(state_t* st)
	{
		std::vector< Wt::WStandardItem* > row;
		Wt::WStandardItem* item;

		item = new Wt::WStandardItem;
		item->setData(st->get_time(), Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).position[0], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).position[1], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).orientation, Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).lin_velocity[0], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).lin_velocity[1], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(st->get_ship_state(0).ang_velocity, Wt::DisplayRole);
		row.push_back(item);

		double speed = magnitude(st->get_ship_state(0).lin_velocity);
		double ang_speed = magnitude(st->get_ship_state(0).ang_velocity);

		item = new Wt::WStandardItem;
		item->setData(speed, Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(ang_speed, Wt::DisplayRole);
		row.push_back(item);

		auto f_and_t = st->get_ship_thruster_sys(0).calc_resultants(st->get_ship_state(0).c_of_mass);

		item = new Wt::WStandardItem;
		item->setData(f_and_t.first[0], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(f_and_t.first[1], Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(magnitude(f_and_t.first), Wt::DisplayRole);
		row.push_back(item);
		item = new Wt::WStandardItem;
		item->setData(f_and_t.second, Wt::DisplayRole);
		row.push_back(item);

		double kinetic = speed * speed + ang_speed * ang_speed;

		item = new Wt::WStandardItem;
		item->setData(kinetic, Wt::DisplayRole);
		row.push_back(item);

		sum_kinetic += kinetic;

		item = new Wt::WStandardItem;
		item->setData(sum_kinetic / (model->rowCount() + 1), Wt::DisplayRole);
		row.push_back(item);

		assert(row.size() == NumColumns);
		return row;
	}

	void on_config_changed()
	{
		std::vector< Wt::Chart::WDataSeries > series;

		for(size_t i = 1; i < NumColumns; ++i)
		{
			if(series_config[i].enabled_box->isChecked())
			{
				series.push_back(Wt::Chart::WDataSeries(i, Wt::Chart::LineSeries));
			}
		}
		chart->setSeries(series);
	}

private:
	Wt::WStandardItemModel* model;
	Wt::Chart::WCartesianChart* chart;
	Wt::WTable* cfg_table;
	std::map< int, series_cfg > series_config;
	double sum_kinetic;
};


#endif


