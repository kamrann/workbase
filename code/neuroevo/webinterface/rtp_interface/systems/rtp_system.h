// rtp_system.h

#ifndef __NE_RTP_SYSTEM_H
#define __NE_RTP_SYSTEM_H

#include "../rtp_defs.h"
#include "../rtp_param.h"

#include <boost/optional.hpp>

#include <array>
#include <string>


namespace Wt {
	class WWidget;
	class WComboBox;
}

enum SystemType {
	NoughtsAndCrosses,
	ShipAndThrusters2D,

	NumSystems,
};

class system_type_param_type: public rtp_param_type
{
public:
	virtual boost::any default_value() const;
	virtual i_param_widget* create_widget() const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;
};

class system_param_type: public rtp_param_type
{
public:
	virtual boost::any default_value() const;
	virtual i_param_widget* create_widget() const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;
};


extern const std::array< std::string, NumSystems > SystemNames;


//Wt::WComboBox* create_system_select_widget();
//Wt::WWidget* create_sim_params_widget();

/*
struct rtp_named_param_list;

struct rtp_system_traits_base
{
	static rtp_named_param_list params(SystemType sys);
};

template < int Sys = NumSystems >
struct rtp_system_traits;

template <>
struct rtp_system_traits< NumSystems >: public rtp_system_traits_base
{};

template <>
struct rtp_system_traits< NoughtsAndCrosses >
{
	static rtp_named_param_list params();
};

template <>
struct rtp_system_traits< ShipAndThrusters2D >
{
	static rtp_named_param_list params();
};
*/


class i_system_drawer;

class i_system
{
public:
	static rtp_named_param_list params(SystemType sys);
	static i_system* create_instance(rtp_param param);

public:
//	virtual boost::optional< agent_id_t > register_agent() = 0;
	virtual void generate_initial_state(rgen_t& rgen) = 0;

//	virtual bool pending_decision(agent_id_t id) = 0;
//	virtual void register_solution_decision(i_decision const& dec) = 0;
	virtual bool update() = 0;

	virtual i_system_drawer* get_drawer() const = 0;
};


#endif

