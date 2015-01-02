// agent_defn.h

#ifndef __NE_SYSSIM_AGENT_DEFN_H
#define __NE_SYSSIM_AGENT_DEFN_H

#include "system_sim_fwd.h"
#include "system_state_values.h"

#include "ddl/ddl.h"

#include <string>
#include <vector>


namespace sys {

	class i_agent_defn
	{
	public:
		virtual std::string get_name() const = 0;
		virtual ddl::defn_node get_spec_defn(ddl::specifier& spc) = 0;
		virtual ddl::defn_node get_instance_defn(ddl::specifier& spc) = 0;

		virtual ddl::dep_function< state_value_id_list >
			get_agent_state_values_fn() const = 0; 
		virtual state_value_id_list get_agent_state_values(ddl::navigator nav) const = 0;
		
		virtual std::vector< std::string > sensor_inputs(ddl::navigator nav) const = 0;

		virtual ddl::dep_function< size_t >
			num_effectors_fn() const = 0;
		virtual size_t num_effectors(ddl::navigator nav) const = 0;

		virtual agent_ptr create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const = 0;

	public:
		virtual ~i_agent_defn() noexcept
		{}
	};

}


#endif

