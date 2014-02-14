// nac_board_coordinates.h

#ifndef __NAC_BOARD_COORDINATES_H
#define __NAC_BOARD_COORDINATES_H

//#include "util/wb_math.hpp"

#include <array>


template <
	size_t Dim
>
struct coord
{
	std::array< int, Dim > _elems;

	coord()
	{}

	coord(coord< Dim - 1 > const& n_1, int const _e)
	{
		std::copy(n_1._elems.begin(), n_1._elems.end(), _elems.begin());
		_elems[Dim - 1] = _e;
	}

	inline int operator[] (size_t const i) const
	{
		return _elems[i];
	}

	inline int& operator[] (size_t const i)
	{
		return _elems[i];
	}

	inline size_t to_flat_index(size_t const Size) const
	{
		size_t mult = 1;
		size_t res = 0;
		for(size_t d = 0; d < Dim; ++d)
		{
			res += _elems[Dim - d - 1] * mult;
			mult *= Size;
		}
		return res;
	}

	static inline coord from_flat_index(size_t index, size_t const Size)
	{
		coord cd;
		for(size_t d = 0; d < Dim; ++d)
		{
			cd[Dim - d - 1] = index % Size;
			index /= Size;
		}
		return cd;
	}

	/*
	inline coord< Dim - 1 > get_sub() const
	{
		coord< Dim - 1 > sub;
		std::copy(_elems.begin(), _elems.begin() + Dim - 1, sub._elems.begin());
		return sub;
	}
	*/

	inline coord get_complement(size_t const Size) const
	{
		coord c;
		for(size_t i = 0; i < Dim; ++i)
		{
			c[i] = Size - 1 - _elems[i];
		}
		return c;
	}

	inline coord operator+ (coord const& other) const
	{
		coord c;
		for(size_t i = 0; i < Dim; ++i)
		{
			c[i] = _elems[i] + other[i];
		}
		return c;
	}

	inline coord operator- (coord const& other) const
	{
		coord c;
		for(size_t i = 0; i < Dim; ++i)
		{
			c[i] = _elems[i] - other[i];
		}
		return c;
	}

	inline coord operator* (int const mult) const
	{
		coord c(*this);
		for(auto& e : c._elems)
		{
			e *= mult;
		}
		return c;
	}

	inline coord operator/ (int const denom) const
	{
		coord c(*this);
		for(auto& e : c._elems)
		{
			e /= denom;
		}
		return c;
	}
};

/*
template <
	size_t Dim
>
struct vector: public coord< Dim >
{};
*/


#endif


