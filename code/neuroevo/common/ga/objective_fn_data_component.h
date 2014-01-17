// objective_fn_data_component.h

#ifndef __OBJFN_DATA_COMP_H
#define __OBJFN_DATA_COMP_H

#include "util/meta_dependencies.h"


// Class from which all objective function data components should be derived
struct ofd_component: public meta_util::dependency_component_base
{
	// Default ofd component has no dependencies and null implementations of update() and finalise()
	typedef mpl::vector0<> dependencies;

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{}

	template < typename ofdata >
	static inline void finalise(ofdata& ofd)
	{}
};


#endif


