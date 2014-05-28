// param_manager.cpp

#include "param_manager.h"
#include "par_wgt.h"

#include <Wt/WFormWidget>


namespace prm
{
	void param_mgr::register_widget(param_wgt_id id, param_wgt* w)
	{
		m_widgets[id] = w;
	}

	bool param_mgr::register_change_handler(param_wgt_id id, boost::function< void() > fn)
	{
		auto it = m_widgets.find(id);
		if(it == m_widgets.end())
		{
			return false;
		}

		it->second->connect_changed_handler(fn);
		return true;
	}

	param param_mgr::retrieve_param(param_wgt_id id) const
	{
		auto it = m_widgets.find(id);
		if(it == m_widgets.end())
		{
			return param();
		}

		return it->second->get_param();
	}
}


