// noughts_and_crosses_system.h

#ifndef __NOUGHTS_AND_CROSSES_SYSTEM_H
#define __NOUGHTS_AND_CROSSES_SYSTEM_H

#include "nac_board_coordinates.h"
#include "nac_board_lines.h"
#include "util/wb_math.hpp"

#include <boost/optional.hpp>

#include <array>
#include <vector>


class noughts_and_crosses_base
{
public:
	enum class Player {
		Noughts,
		Crosses,
	};

	enum class SquareState {
		Unplayed = 0,
		Nought = 1,
		Cross = 2,
	};

	static inline Player other_player(Player p)
	{
		return p == Player::Noughts ? Player::Crosses : Player::Noughts;
	}

	static inline SquareState player_marker(Player p)
	{
		return p == Player::Noughts ? SquareState::Nought : SquareState::Cross;
	}
};

template <
	size_t Dimensions_,
	size_t Size_
>
class noughts_and_crosses_system: public noughts_and_crosses_base
{
public:
	enum {
		Dimensions = Dimensions_,
		Size = Size_,
		NumSquares = wb::powerT<>::ct< Size, Dimensions >::res,
	};

	class board_state
	{
	public:
		// Square coordinate. Each element is in [0, Size - 1]
		typedef coord< Dimensions > coord_t;

		typedef std::array< coord_t, Size > line;

	private:
		typedef std::array< SquareState, NumSquares > squares_t;
		squares_t squares;

		// Direction vector, with each element restricted to {0, 1}
		typedef coord< Dimensions > vector_t;

		struct dim_accessor_base
		{
			squares_t const& data;
			size_t const offset;

			dim_accessor_base(squares_t const& _data, size_t const _offset = 0): data(_data), offset(_offset)
			{}
		};

	public:
		template < size_t Dim >
		struct dim_accessor: dim_accessor_base
		{
			using dim_accessor_base::dim_accessor_base;

			inline dim_accessor< Dim - 1 > operator[] (size_t const i) const
			{
				return dim_accessor< Dim - 1 >(data, offset + i * wb::powerT<>::ct< Size, Dim - 1 >::res);
			}
		};

		template <>
		struct dim_accessor< 0 >: dim_accessor_base
		{
			using dim_accessor_base::dim_accessor_base;

			inline operator SquareState()
			{
				return data[offset];
			}
		};

	public:
		board_state()
		{
			reset();
		}

		inline dim_accessor< Dimensions - 1 > operator[] (size_t const i) const
		{
			return dim_accessor< Dimensions >(squares)[i];
		}

		inline SquareState by_index(size_t const i) const
		{
			return squares[i];
		}

		inline SquareState& by_index(size_t const i)
		{
			return squares[i];
		}

		inline SquareState by_coord(coord_t const& cd) const
		{
			return by_index(cd.to_flat_index(Size));
		}

		inline SquareState& by_coord(coord_t const& cd)
		{
			return by_index(cd.to_flat_index(Size));
		}

	public:
		inline void reset()
		{
			std::fill(squares.begin(), squares.end(), SquareState::Unplayed);
		}

		inline size_t num_unplayed_squares() const
		{
			size_t count = 0;
			for(SquareState ss : squares)
			{
				if(ss == SquareState::Unplayed)
				{
					++count;
				}
			}
			return count;
		}

		inline size_t num_played_squares() const
		{
			return NumSquares - num_unplayed_squares();
		}

		inline bool make_play(size_t sq_idx, Player player)
		{
			if(by_index(sq_idx) == SquareState::Unplayed)
			{
				by_index(sq_idx) = (player == Player::Noughts ? SquareState::Nought : SquareState::Cross);
				return true;
			}
			else
			{
				return false;
			}
		}

		boost::optional< Player > line_is_complete(line const& ln) const
		{
			boost::optional< SquareState > pl;
			for(coord_t const& cd: ln)
			{
				SquareState ss = by_coord(cd);
				if(ss == SquareState::Unplayed || pl && ss != *pl)
				{
					return boost::optional< Player >();
				}
				pl = ss;
			}
			return *pl == SquareState::Nought ? Player::Noughts : Player::Crosses;
		}

		std::vector< line > gen_all_lines() const
		{
			std::vector< line_defn< Dimensions > > ldefs = get_all_lines< Dimensions >(Size);
			std::vector< line > lines;
			for(auto const& def: ldefs)
			{
				line ln;
				for(size_t i = 0; i < Size; ++i)
				{
					ln[i] = def.base + def.dir * i;

					for(size_t d = 0; d < Dimensions; ++d)
					{
						assert(ln[i][d] < Size);
					}
				}
				lines.push_back(ln);
			}
			return lines;
		}

		size_t get_line_mark_count(line const& ln, SquareState mark) const
		{
			size_t count = 0;
			for(auto const& c : ln)
			{
				if(by_coord(c) == mark)
				{
					++count;
				}
			}
			return count;
		}

		std::vector< line > get_almost_complete_lines(Player player, size_t max_missing = 1) const
		{
			std::vector< line > all = gen_all_lines();
			std::vector< line > result;
			for(line const& ln : all)
			{
				size_t player_count = get_line_mark_count(ln, player_marker(player));
				size_t opponent_count = get_line_mark_count(ln, player_marker(other_player(player)));

				if(opponent_count == 0 && Size - player_count <= max_missing)
				{
					result.push_back(ln);
				}
			}
			return result;
		}

		boost::optional< Player > is_winner() const
		{
			std::vector< line > lines = gen_all_lines();
			for(auto const& ln : lines)
			{
				auto pl = line_is_complete(ln);
				if(pl)
				{
					return pl;
				}
			}
			return boost::optional< Player >();
		}

		boost::optional< line > get_winning_line() const
		{
			std::vector< line > lines = gen_all_lines();
			for(auto const& ln : lines)
			{
				auto pl = line_is_complete(ln);
				if(pl)
				{
					return ln;
				}
			}
			return boost::optional< line >();
		}

		inline bool game_over() const
		{
			return num_unplayed_squares() == 0 || is_winner();
		}

		inline boost::optional< Player > next_to_act() const
		{
			size_t const num_played = num_played_squares();
			if(num_played == NumSquares)
			{
				return boost::optional< Player >();
			}
			else
			{
				return num_played % 2 == 0 ? Player::Crosses : Player::Noughts;
			}
		}
	};

	typedef boost::optional< size_t > solution_result;		// 0-based index of the square played, or boost::none for failure to make valid play

public:
//	void register_solution_result(solution_result const& res);
//	void update();
};


#endif


