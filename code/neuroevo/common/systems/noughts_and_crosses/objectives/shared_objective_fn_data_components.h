// shared_objective_fn_data_components.h

#ifndef __NAC_SHARED_OBJFN_DATA_COMPS_H
#define __NAC_SHARED_OBJFN_DATA_COMPS_H

#include "ga/objective_fn_data_component.h"


struct num_moves_ofd: public ofd_component
{
	size_t num_moves;

	num_moves_ofd(): num_moves(0)
	{}
	
	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		++ofd.num_moves;
	}
};

struct made_invalid_ofd: public ofd_component
{
	bool made_invalid_move;

	made_invalid_ofd(): made_invalid_move(false)
	{}

	template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
	static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
	{
		ofd.made_invalid_move = ofd.made_invalid_move || dec == boost::none;
	}
};


#endif


