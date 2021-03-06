// rtp_param.h

#ifndef __NE_RTP_PARAM_H
#define __NE_RTP_PARAM_H

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>


typedef boost::any rtp_param;
typedef std::vector< rtp_param > rtp_param_list;
typedef rtp_param_list::const_iterator rtp_param_list_it;
typedef std::pair< rtp_param, rtp_param > rtp_param_pair;


class i_param_widget;
class rtp_param_manager;

class rtp_param_type
{
public:
	virtual boost::any default_value() const = 0;
	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const = 0;
	virtual rtp_param get_widget_param(i_param_widget const* w) const = 0;

public:
	virtual ~rtp_param_type() = default;
};


struct rtp_named_param
{
	//boost::shared_ptr< rtp_param_type > type;
	rtp_param_type* type;
	std::string name;

	rtp_named_param(rtp_param_type* _type = nullptr, std::string const& _name = ""): type(_type), name(_name)
	{}
};

struct rtp_named_param_list: public std::vector< rtp_named_param >
{
};



#endif


