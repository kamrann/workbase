// rtp_system.h

#ifndef __NE_RTP_SYSTEM_H
#define __NE_RTP_SYSTEM_H

#include "../rtp_defs.h"
#include "../rtp_param.h"
#include "../params/enum_par.h"
#include "../params/paramlist_par.h"

//
#include "../rtp_observer.h"
//

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <array>
#include <string>


namespace Wt {
	class WWidget;
	class WComboBox;
}

enum SystemType {
//	NoughtsAndCrosses,
//	ShipAndThrusters2D,
	Physics2D,

	NumSystems,
};

class system_type_param_type: public rtp_enum_param_type
{
public:
	system_type_param_type();
};

class system_param_type: public rtp_paramlist_param_type
{
public:
	system_param_type(bool evolvable = false);

public:
	virtual size_t provide_num_child_params(rtp_param_manager* mgr) const;
	virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const;

private:
	bool m_evolvable;
};


extern const std::array< std::string, NumSystems > SystemNames;


class i_agent;
class i_observer;
class i_population_wide_observer;
class i_genome_mapping;
class i_agent_factory;
class i_properties;
class i_property_values;
class i_system_drawer;
class interactive_input;

class i_system
{
public:
	static rtp_param_type* params(bool evolvable = false);
	static rtp_param_type* params(SystemType sys, bool evolvable = false);
	static std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > create_instance(rtp_param param, bool evolvable = false);

public:
	virtual boost::any generate_initial_state(rgen_t& rgen) const = 0;
	virtual void set_state(boost::any const& st) = 0;

	virtual void clear_agents() = 0;
	virtual boost::optional< agent_id_t > register_agent(boost::shared_ptr< i_agent > a) = 0;

	virtual void register_interactive_input(interactive_input const& input) = 0;
	virtual bool update(i_observer* obs = nullptr) = 0;
	virtual i_observer::observations_t record_observations(i_observer* obs) const = 0;

	virtual boost::shared_ptr< i_properties const > get_state_properties() const = 0;
	virtual boost::shared_ptr< i_property_values const > get_state_property_values() const = 0;

	virtual i_system_drawer* get_drawer() const = 0;
};



#endif

