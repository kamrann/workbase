// properties_chart_widget.h

#ifndef __PROPERTIES_CHART_WIDGET_H
#define __PROPERTIES_CHART_WIDGET_H

#include "../rtp_interface/rtp_properties.h"

#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WApplication>
#include <Wt/WTable>
#include <Wt/WCheckBox>
#include <Wt/WHBoxLayout>

#include <boost/math/constants/constants.hpp>


class properties_chart_widget: public Wt::WContainerWidget
{
public:
	struct series_cfg
	{
		Wt::WCheckBox* enabled_box;
	};

public:
	properties_chart_widget(): model(nullptr)
	{
//		setLayoutSizeAware(true);

		Wt::WHBoxLayout* layout = new Wt::WHBoxLayout;
		setLayout(layout);

		model = new Wt::WStandardItemModel();
		

		chart = new Wt::Chart::WCartesianChart(this);
		chart->setBackground(Wt::WColor(220, 220, 220));
		chart->setModel(model);
		chart->setXSeriesColumn(0);	// TODO: Assuming that every system puts Time as its first state property...
		chart->setLegendEnabled(true);
		chart->setType(Wt::Chart::ScatterPlot);

		/*
		* Provide ample space for the title, the X and Y axis and the legend.
		*/
		chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
		chart->setPlotAreaPadding(120, Wt::Right);

		chart->resize(500, 400);
		chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
		layout->addWidget(chart, 1);

		cfg_table = new Wt::WTable(this);
		cfg_table->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
		layout->addWidget(cfg_table, 0, Wt::AlignMiddle);

		reset();
	}

public:
	void clear_content()
	{
		model->removeRows(0, model->rowCount());
	}

	void reset(boost::shared_ptr< i_properties const > props = nullptr)
	{
		model->clear();
		cfg_table->clear();

		if(props)
		{
			size_t count = props->num_properties();
			model->insertColumns(0, count);
			for(size_t i = 0; i < count; ++i)
			{
				model->setHeaderData(i, props->get_property_name(i));
			}

			for(int i = 1; i < model->columnCount(); ++i)
			{
				new Wt::WText(Wt::asString(model->headerData(i)), cfg_table->elementAt(i - 1, 0));

				series_cfg& sc = series_config[i];
				sc.enabled_box = new Wt::WCheckBox(cfg_table->elementAt(i - 1, 1));
				sc.enabled_box->changed().connect(this, &properties_chart_widget::on_config_changed);
			}

			cfg_table->enable();
		}
		else
		{
			cfg_table->disable();
		}
		
		m_props = props;
	}
	
	void append_data(boost::shared_ptr< i_property_values const > vals)
	{
		model->appendRow(create_data_row(m_props, vals));
		chart->update();
	}

protected:
	std::vector< Wt::WStandardItem* > create_data_row(boost::shared_ptr< i_properties const >& props, boost::shared_ptr< i_property_values const > vals)
	{
		std::vector< Wt::WStandardItem* > row;
		Wt::WStandardItem* item;

		size_t num_props = props->num_properties();
		for(size_t i = 0; i < num_props; ++i)
		{
			std::string name = props->get_property_name(i);
			boost::any val = vals->get_property(name);
			
			item = new Wt::WStandardItem;
			item->setData(val, Wt::DisplayRole);
			row.push_back(item);
		}

		return row;
	}

	void on_config_changed()
	{
		std::vector< Wt::Chart::WDataSeries > series;

		size_t num_props = m_props->num_properties();
		for(size_t i = 1; i < num_props; ++i)
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
	boost::shared_ptr< i_properties const > m_props;
};


#endif


