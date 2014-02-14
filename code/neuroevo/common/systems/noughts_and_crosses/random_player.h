// random_player.h

#ifndef __NAC_RANDOM_PLAYER_H
#define __NAC_RANDOM_PLAYER_H

#include "noughts_and_crosses_system.h"

#include <boost/random/uniform_int_distribution.hpp>


template <
	size_t Dimensions,
	size_t Size,
	typename RGen
>
inline size_t get_random_move(
	typename noughts_and_crosses_system< Dimensions, Size >::board_state const& board,
	typename noughts_and_crosses_system< Dimensions, Size >::Player as_player,
	RGen& rgen
	)
{
	boost::random::uniform_int_distribution<> dist(0, noughts_and_crosses_system< Dimensions, Size >::NumSquares - 1);
	size_t sq;
	do
	{
		sq = dist(rgen);
	}
	while(board.by_index(sq) != noughts_and_crosses_base::SquareState::Unplayed);

	return sq;
}

template <
	size_t Dimensions,
	size_t Size,
	typename RGen
>
inline void make_random_move(
	typename noughts_and_crosses_system< Dimensions, Size >::board_state& board,
	typename noughts_and_crosses_system< Dimensions, Size >::Player as_player,
	RGen& rgen
	)
{
	board.make_play(get_random_move(board, as_player, rgen), as_player);
}


#endif


