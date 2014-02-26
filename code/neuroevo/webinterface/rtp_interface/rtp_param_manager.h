// rtp_param_manager.h

#ifndef __NE_RTP_PARAM_MANAGER_H
#define __NE_RTP_PARAM_MANAGER_H

#include "rtp_param.h"

#include <boost/function.hpp>

#include <map>
#include <string>


class i_param_widget;

class rtp_param_manager
{
public:
	void register_widget(std::string name, i_param_widget* w);
	bool register_change_handler(std::string name, boost::function< void() > fn);
	rtp_param retrieve_param(std::string name) const;

private:
	std::map< std::string, i_param_widget* > m_widgets;
};


#endif


