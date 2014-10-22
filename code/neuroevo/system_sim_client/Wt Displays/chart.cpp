// chart.cpp

#include "chart.h"

#include <Wt/WVBoxLayout>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>


chart_display::chart_display(chart_dd const& dd)
{
	auto _layout = new Wt::WVBoxLayout;
	setLayout(_layout);

	m_model = new Wt::WStandardItemModel();

	auto chart = new Wt::Chart::WCartesianChart{};

	chart->setBackground(Wt::WColor(220, 220, 220));
	chart->setModel(m_model);
	chart->setXSeriesColumn(0);
	chart->setLegendEnabled(true);
	chart->setType(Wt::Chart::ScatterPlot);

	/*
	* Provide ample space for the title, the X and Y axis and the legend.
	*/
	chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
	chart->setPlotAreaPadding(120, Wt::Right);

	chart->resize(500, 400);
	chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);

	_layout->addWidget(chart);

	
	auto num_series = dd.series.size();
	m_model->insertColumns(0, num_series + 1);
	std::vector< Wt::Chart::WDataSeries > chart_series;
	for(size_t i = 0; i < num_series; ++i)
	{
		m_model->setHeaderData(i + 1, dd.series[i].name);
		chart_series.push_back(Wt::Chart::WDataSeries(i + 1, Wt::Chart::LineSeries));
	}

	chart->setSeries(chart_series);
}

void chart_display::update_from_data(display_data const& dd)
{
	auto const& data = boost::any_cast<chart_dd const&>(dd.data);

	if(data.reload)
	{
		m_model->removeRows(0, m_model->rowCount());
	}

	// Note: Currently assuming every series has a datapoint for every x value
	auto existing_count = m_model->rowCount();
	auto count = data.x_vals.size();
	auto num_series = data.series.size();	// Assuming unchanged from chart construction!

	for(size_t i = existing_count; i < count; ++i)
	{
		auto row = std::vector < Wt::WStandardItem* > {};
		auto x = new Wt::WStandardItem{};
		x->setData(data.x_vals[i], Wt::DisplayRole);
		row.push_back(x);
		for(size_t s = 0; s < num_series; ++s)
		{
			auto y = new Wt::WStandardItem{};
			y->setData(data.series[s].y_vals[i], Wt::DisplayRole);
			row.push_back(y);
		}

		m_model->appendRow(row);
	}
}


