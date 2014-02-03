// iterative_junction_gen.h

#ifndef __GK_ITERATIVE_JUNCTION_GEN_H
#define __GK_ITERATIVE_JUNCTION_GEN_H

#include "graph_defs/graph_defn.h"


namespace iter_junc {

	struct graph_data
	{
		gk::graph const& g;
		gk::node_coord_map_3d const& node_positions;

		graph_data(gk::graph const& _g, gk::node_coord_map_3d const& _positions):
			g(_g), node_positions(_positions)
		{}
	};

	class iterative_junction_gen
	{
	public:
	};

}


#endif


