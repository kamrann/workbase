// rtp_param.h

#ifndef __NE_RTP_PARAM_H
#define __NE_RTP_PARAM_H

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>


class i_param_widget;


typedef boost::any rtp_param;
typedef std::vector< rtp_param > rtp_param_list;
typedef rtp_param_list::const_iterator rtp_param_list_it;


class rtp_param_type
{
public:
	virtual boost::any default_value() const = 0;
	virtual i_param_widget* create_widget() const = 0;
	virtual rtp_param get_widget_param(i_param_widget const* w) const = 0;
};


struct rtp_named_param
{
	boost::shared_ptr< rtp_param_type > type;
	std::string name;

	rtp_named_param(rtp_param_type* _type = nullptr, std::string const& _name = ""): type(_type), name(_name)
	{}
};

struct rtp_named_param_list: public std::vector< rtp_named_param >
{
};



#endif


