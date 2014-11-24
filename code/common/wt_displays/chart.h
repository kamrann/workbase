// chart.h

#ifndef __WB_WTDISP_CHART
#define __WB_WTDISP_CHART

#include "wt_cmdline_server/display_base.h"
#include "wt_cmdline_server/display_defn.h"

#include <Wt/WContainerWidget>

#include <string>
#include <vector>


struct chart_dd
{
	std::string x_axis_name;
	std::vector< boost::any > x_vals;

	struct series_data
	{
		std::string name;
		std::vector< double > y_vals;
	};

	std::vector< series_data > series;

	bool reload;	// if true, chart reloads all the data; otherwise, just adds on any beyond its existing data point count

	chart_dd(): reload{ false }
	{}
};


class chart_display:
	public Wt::WContainerWidget,
	public display_base
{
public:
	chart_display(chart_dd const& dd);

public:
	virtual Wt::WWidget* get_wt_widget() override;
	virtual void update_from_data(boost::any const& dd) override;

private:
	Wt::WStandardItemModel* m_model;
};


class chart_display_defn:
	public display_defn
{
public:
	static const std::string ID;

	virtual std::string name() const override;
	virtual display_base* create(boost::any const& data) const override;
};



#endif


