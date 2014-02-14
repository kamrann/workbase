// play_to_completion.h

#ifndef __NAC_PLAY_TO_COMPLETION_H
#define __NAC_PLAY_TO_COMPLETION_H

#include "../noughts_and_crosses_system.h"
#include "../random_player.h"
#include "../sensible_player.h"

#include <chrono>

/*
Agent is presented with a game state and must play to completion.
An invalid move choice will terminate the game prematurely.
*/

template <
	size_t Dimensions_,
	size_t Size_,
	bool AutoOpponent,	// TODO: Don't think this should remain as template parameter
	typename RGen	// See comment below
>
class play_to_completion
{
public:
	enum {
		Dimensions = Dimensions_,
		Size = Size_,
	};

	typedef noughts_and_crosses_system< Dimensions, Size > system_t;
	// TODO: Allow subset definition here, or define wholly within system???
//	typedef system_t::board_state solution_input;

	/*
	Structure for state of a single agent.
	*/
	struct agent_state {};

	/*
	This structure represents the state of the problem domain/environment, excluding that of the contained agents.
	*/
	typedef typename system_t::board_state envt_state;

	/*
	This is the state known to a given agent, and represents all the information available to that agent to make decisions.
	*/
	typedef envt_state agent_sensor_state, solution_input;

	/*
	Overall state, environment plus all agents.
	*/
	struct state: envt_state
	{
		std::vector< agent_state > agents;

		// TODO: Think this should be stored elsewhere. It's used by the system simulation itself, but is not actually a
		// part of the system being simulated...
		RGen rgen;
		//

		inline agent_sensor_state get_sensor_state(size_t iagent) const
		{
			return agent_sensor_state(*this);
		}
	};


	template < typename RGen >
	static inline void init_state(size_t num_agents, state& st, RGen& rgen)
	{
		st = state();
		st.agents.resize(num_agents);	// TODO: this should be tied to register_agent()

		//
		st.rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		//
	}

	// TODO: should be part of system?
	struct system_agent_config
	{
		// Perhaps after agents are registered, this can be used to setup up the agent section of the system state??
		// (Generally - obviously for 2 player noughts and crosses, it contains no info)
		std::array< bool, 2 > registered;	// indexed by system_t::Player enum

		system_agent_config(): registered{ { false, false } }
		{}
	};

	typedef typename system_t::Player agent_id_t;

	// TODO: Could typedef an agent type also, (which in this case would be same as id type) and allow to specify which type to register
	static boost::optional< agent_id_t > register_agent(system_agent_config& sa_cfg)
	{
		if(!sa_cfg.registered[(int)agent_id_t::Crosses])
		{
			sa_cfg.registered[(int)agent_id_t::Crosses] = true;
			return agent_id_t::Crosses;
		}
		else if(!sa_cfg.registered[(int)agent_id_t::Noughts])
		{
			sa_cfg.registered[(int)agent_id_t::Noughts] = true;
			return agent_id_t::Noughts;
		}
		else
		{
			return boost::optional< agent_id_t >();
		}
	}

	static inline bool pending_decision(agent_id_t a_id, typename system_t::board_state const& game_state)
	{
		if(game_state.game_over())
		{
			return false;
		}

		size_t num_played = game_state.num_played_squares();
		if(a_id == system_t::Player::Crosses)
		{
			return num_played % 2 == 0;
		}
		else
		{
			return num_played % 2 == 1;
		}
	}

	// This will only exist for systems/scenarios where only one agent can ever act at a time
	static boost::optional< agent_id_t > pending_agent(typename system_t::board_state const& game_state)
	{
		if(game_state.game_over())
		{
			return boost::optional< agent_id_t >();
		}

		size_t num_played = game_state.num_played_squares();
		if(num_played % 2 == 0)
		{
			return system_t::Player::Crosses;
		}
		else
		{
			return system_t::Player::Noughts;
		}
	}

	struct system_update_params
	{
		std::vector< typename system_t::solution_result > decisions;
	};

	static void register_solution_decision(typename system_t::solution_result const& res, system_update_params& sup)
	{
		sup.decisions.push_back(res);
	}
	
	static bool update(state& st, system_update_params const& sup)
	{
		assert(sup.decisions.size() == 1);

		if(sup.decisions[0])
		{
			assert(st.by_index(*sup.decisions[0]) == system_t::SquareState::Unplayed);

			// Mark cross in chosen square
			boost::optional< system_t::Player > next = st.next_to_act();
			assert(next);
			system_t::SquareState marker = noughts_and_crosses_base::player_marker(*next);
			st.by_index(*sup.decisions[0]) = marker;

			if(AutoOpponent)
			{
				if(st.game_over())
				{
					return false;
				}

				make_sensible_move< system_t::Dimensions, system_t::Size >(st, system_t::Player::Noughts, st.rgen);
			}

			return !st.game_over();
		}
		else
		{
			// Invalid move
			return false;
		}
	}
};


#endif


