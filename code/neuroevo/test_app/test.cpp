// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/mpl/front.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/print.hpp>

#include <boost/type_traits/is_same.hpp>

#include "util/pareto_ct.h"


namespace mpl = boost::mpl;
namespace fus = boost::fusion;
using boost::is_same;


int _tmain(int argc, _TCHAR* argv[])
{
	typedef mpl::vector< int, double > tl;
	typedef pareto< tl > pareto_t;

	size_t const N = 3;
	pareto_t p[N];

	fus::at_c< 0 >(p[0]) = 5;
	fus::at_c< 1 >(p[0]) = 2.0;

	fus::at_c< 0 >(p[1]) = 10;
	fus::at_c< 1 >(p[1]) = 1.0;

	fus::at_c< 0 >(p[2]) = 10;
	fus::at_c< 1 >(p[2]) = 1.5;

	size_t ranks[N] = { 0 };

	pareto_t::calc_dominance_ranks(p, p + N, ranks);

	return 0;
}

