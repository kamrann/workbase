// dimensionality_par.h

#ifndef __NE_RTP_DIM_PARAM_H
#define __NE_RTP_DIM_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"

#include "util/dimensionality.h"

#include <Wt/WComboBox>


class rtp_dimensionality_param_type: public rtp_param_type
{
public:
	rtp_dimensionality_param_type()
	{}

public:
	virtual boost::any default_value() const
	{
		return WorldDimensionality::dim2D;
	}

	i_param_widget* create_widget(rtp_param_manager* mgr) const
	{
		rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);
		box->addItem("2D");
		box->model()->setData(0, 0, WorldDimensionality::dim2D, Wt::UserRole);
		//box->addItem("3D");	TODO:
		//box->model()->setData(1, 0, WorldDimensionality::dim3D, Wt::UserRole);
		return box;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
		Wt::WAbstractItemModel* model = box->model();
		return rtp_param(model->data(model->index(box->currentIndex(), 0), Wt::UserRole));
	}
};


#endif


