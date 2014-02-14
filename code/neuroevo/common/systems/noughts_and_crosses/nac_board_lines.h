// nac_board_lines.h

#ifndef __NAC_BOARD_LINES_H
#define __NAC_BOARD_LINES_H

#include "nac_board_coordinates.h"

#include <vector>


template <
	size_t Dim
>
struct line_defn
{
	coord< Dim > base;
	//vector< Dim > dir;
	coord< Dim > dir;
};

template <
	size_t Dim
>
inline std::vector< coord< Dim > > get_hyperplane_corners(size_t const Size)
{
	std::vector< coord< Dim - 1 > > n_1 = get_hyperplane_corners< Dim - 1 >(Size);
	std::vector< coord< Dim > > corners;
	for(auto const& sub: n_1)
	{
		corners.push_back(coord< Dim >(sub, 0));
		corners.push_back(coord< Dim >(sub, Size - 1));
	}
	return corners;
}

template <>
inline std::vector< coord< 1 > > get_hyperplane_corners< 1 >(size_t const Size)
{
	std::vector< coord< 1 > > corners;
	coord< 1 > c;
	c[0] = 0;
	corners.push_back(c);
	c[0] = Size - 1;
	corners.push_back(c);
	return corners;
}

template <
	size_t Dim
>
inline std::vector< line_defn< Dim > > get_all_lines(size_t const Size)
{
	std::vector< line_defn< Dim - 1 > > n_minus_1 = get_all_lines< Dim - 1 >(Size);
	std::vector< line_defn< Dim > > result;
	for(size_t D = 0; D < Dim; ++D)
	{
		for(size_t L = 0; L < Size; ++L)
		{
			// D & L define a hyperplane within the Dim dimensional space.
			// For every line in Dim - 1 dimensional space, map that line into this hyperplane
			for(size_t n_1_ln = 0; n_1_ln < n_minus_1.size(); ++n_1_ln)
			{
				line_defn< Dim > ln;
				size_t i = 0;
				for(size_t c = 0; c < Dim; ++c)
				{
					if(c == D)
					{
						ln.base[D] = L;
						ln.dir[D] = 0;
					}
					else
					{
						ln.base[c] = n_minus_1[n_1_ln].base[i];
						ln.dir[c] = n_minus_1[n_1_ln].dir[i];
						++i;
					}
				}
				result.push_back(ln);
			}
		}
	}

	// Finally, need to add new diagonals
	std::vector< coord< Dim - 1 > > hyperplane_corners = get_hyperplane_corners< Dim - 1 >(Size);
	for(auto const& c: hyperplane_corners)
	{
		line_defn< Dim > ln;
		ln.base = coord< Dim >(c, 0);
		ln.dir = ln.base.get_complement(Size) - ln.base;
		ln.dir = ln.dir / (Size - 1);
		result.push_back(ln);
	}

	return result;
}

template <>
inline std::vector< line_defn< 1 > > get_all_lines< 1 >(size_t const Size)
{
	std::vector< line_defn< 1 > > result;
	line_defn< 1 > ln;
	ln.base[0] = 0;
	ln.dir[0] = 1;
	result.push_back(ln);
	return result;
}


#endif


