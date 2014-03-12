// enum_par.cpp

#include "enum_par.h"
#include "../rtp_param_widget.h"

#include <Wt/WComboBox>


i_param_widget* rtp_enum_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WComboBox >* box = new rtp_param_widget< Wt::WComboBox >(this);
	for(size_t i = 0; i < m_data.size(); ++i)
	{
		box->addItem(m_data[i].label);
		box->model()->setData(i, 0, m_data[i].val, Wt::UserRole);

		if(i == m_default_idx)
		{
			box->setCurrentIndex(i);
		}
	}
	return box;
}

rtp_param rtp_enum_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WComboBox const* box = (Wt::WComboBox const*)w->get_wt_widget();
	Wt::WAbstractItemModel const* model = box->model();
	return rtp_param(model->data(box->currentIndex(), 0, Wt::UserRole));
}

void rtp_enum_param_type::add_item(std::string name, boost::any val, std::string desc)
{
	m_data.push_back(item_data(name, val, desc));
}

void rtp_enum_param_type::set_default_index(size_t idx)
{
	m_default_idx = idx;
}





