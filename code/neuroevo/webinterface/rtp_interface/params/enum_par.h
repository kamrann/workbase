// enum_par.h

#ifndef __NE_RTP_ENUM_PARAM_H
#define __NE_RTP_ENUM_PARAM_H

#include "../rtp_param.h"

#include <boost/any.hpp>

#include <vector>
#include <string>


class i_param_widget;

class rtp_enum_param_type: public rtp_param_type
{
public:
	rtp_enum_param_type()
	{
		m_default_idx = 0;
	}

public:
	virtual boost::any default_value() const
	{
		return m_data[m_default_idx].val;
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;

	void add_item(std::string name, boost::any val, std::string desc = "");
	void set_default_index(size_t idx);

private:
	size_t m_default_idx;

	struct item_data
	{
		std::string label;
		boost::any val;
		std::string desc;

		item_data(std::string _label = "", boost::any _val = boost::any(), std::string _desc = ""):
			label(_label), val(_val), desc(_desc)
		{}
	};

	std::vector< item_data > m_data;
};


#endif


