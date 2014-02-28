// rtp_system.h

#ifndef __NE_RTP_SYSTEM_H
#define __NE_RTP_SYSTEM_H

#include "../rtp_defs.h"
#include "../rtp_param.h"
#include "../params/nestedparam_par.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

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
	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const;
	virtual rtp_param get_widget_param(i_param_widget const* w) const;
};

class system_param_type: public rtp_autonestedparam_param_type
{
public:
	system_param_type(bool evolvable = false);

public:
	virtual rtp_named_param provide_selection_param() const;
	virtual rtp_param_type* provide_nested_param(rtp_param_manager* mgr) const;

private:
	bool m_evolvable;
};


extern const std::array< std::string, NumSystems > SystemNames;


class i_agent;
class i_observer;
class i_genome_mapping;
class i_agent_factory;
class i_properties;
class i_property_values;
class i_system_drawer;

class i_system
{
public:
	static rtp_named_param_list params(SystemType sys, bool evolvable = false);
	static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* > create_instance(rtp_param param, bool evolvable = false);

public:
	virtual boost::any generate_initial_state(rgen_t& rgen) const = 0;
	virtual void set_state(boost::any const& st) = 0;

	virtual void clear_agents() = 0;
	virtual boost::optional< agent_id_t > register_agent(i_agent* a) = 0;

	virtual bool update(i_observer* obs = nullptr) = 0;
	virtual boost::any record_observations(i_observer* obs) const = 0;

	virtual boost::shared_ptr< i_properties const > get_state_properties() const = 0;
	virtual boost::shared_ptr< i_property_values const > get_state_property_values() const = 0;

	virtual i_system_drawer* get_drawer() const = 0;
};


#endif

