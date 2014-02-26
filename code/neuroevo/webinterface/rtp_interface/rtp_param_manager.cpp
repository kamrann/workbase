// rtp_param_manager.cpp

#include "rtp_param_manager.h"
#include "rtp_param_widget.h"

#include <Wt/WFormWidget>


void rtp_param_manager::register_widget(std::string name, i_param_widget* w)
{
	m_widgets[name] = w;
}

bool rtp_param_manager::register_change_handler(std::string name, boost::function< void() > fn)
{
	auto it = m_widgets.find(name);
	if(it == m_widgets.end())
	{
		return false;
	}

	Wt::WFormWidget* fw = dynamic_cast<Wt::WFormWidget*>(it->second->get_wt_widget());
	if(fw == nullptr)
	{
		return false;
	}

	fw->changed().connect(std::bind(fn));
	return true;
}

rtp_param rtp_param_manager::retrieve_param(std::string name) const
{
	auto it = m_widgets.find(name);
	if(it == m_widgets.end())
	{
		return rtp_param();
	}

	return it->second->get_param();
}



