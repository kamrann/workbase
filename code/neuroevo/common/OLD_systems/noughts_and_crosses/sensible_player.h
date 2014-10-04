// sensible_player.h

#ifndef __NAC_SENSIBLE_PLAYER_H
#define __NAC_SENSIBLE_PLAYER_H

#include "noughts_and_crosses_system.h"


template <
	size_t Dimensions,
	size_t Size,
	typename RGen
>
inline size_t get_sensible_move(
	typename noughts_and_crosses_system< Dimensions, Size >::board_state const& board,
	typename noughts_and_crosses_system< Dimensions, Size >::Player as_player,
	RGen& rgen)
{
	auto almost_complete = board.get_almost_complete_lines(as_player, 1);
	if(!almost_complete.empty())
	{
		for(auto const& c : almost_complete[0])
		{
			if(board.by_coord(c) == noughts_and_crosses_base::SquareState::Unplayed)
			{
				return c.to_flat_index(Size);
			}
		}
	}

	almost_complete = board.get_almost_complete_lines(noughts_and_crosses_base::other_player(as_player), 1);
	if(!almost_complete.empty())
	{
		for(auto const& c : almost_complete[0])
		{
			if(board.by_coord(c) == noughts_and_crosses_base::SquareState::Unplayed)
			{
				return c.to_flat_index(Size);
			}
		}
	}

	return get_random_move< Dimensions, Size >(board, as_player, rgen);
}

template <
	size_t Dimensions,
	size_t Size,
	typename RGen
>
inline void make_sensible_move(
	typename noughts_and_crosses_system< Dimensions, Size >::board_state& board,
	typename noughts_and_crosses_system< Dimensions, Size >::Player as_player,
	RGen& rgen
	)
{
	board.make_play(get_sensible_move< Dimensions, Size >(board, as_player, rgen), as_player);
}


#endif


