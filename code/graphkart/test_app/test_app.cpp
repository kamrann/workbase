// test.cpp : Defines the entry point for the console application.
//


#include "graphgen/gen_planar.h"
#include "draw_graph/text_output.h"

#include <boost/random/mersenne_twister.hpp>

#include <chrono>
#include <iostream>

#include <tchar.h>


int _tmain(int argc, _TCHAR* argv[])
{
	boost::random::mt19937 rgen;
	rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));

	gk::graph g;
	gk::ggen::generate_planar_graph(0, 0, g, rgen);

	std::cout << "g:" << std::endl;
	std::cout << g;

	return 0;
}

