// param_manager.h

#ifndef __WB_PARAM_MANAGER_H
#define __WB_PARAM_MANAGER_H

#include "params/param.h"

#include <boost/function.hpp>

#include <map>
#include <string>


namespace prm
{
	class param_wgt;

	typedef std::string param_wgt_id;

	class param_mgr
	{
	public:
		void register_widget(param_wgt_id id, param_wgt* w);
		bool register_change_handler(param_wgt_id id, boost::function< void() > fn);
		param retrieve_param(param_wgt_id id) const;

	private:
		std::map< param_wgt_id, param_wgt* > m_widgets;
	};
}


#endif


