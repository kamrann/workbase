// text_output.h

#ifndef __GK_TEXT_OUTPUT_H
#define __GK_TEXT_OUTPUT_H

#include "graph_defs/graph_defn.h"

#include <ostream>


// NOTE: Since currently gk::graph is just a typedef, we need to put this overload into the namespace of adjacency_list (boost)
namespace boost {

	template <
		class _Elem,
		class _Traits,
		class _Graph
	>
	inline std::basic_ostream< _Elem, _Traits >& operator<< (std::basic_ostream< _Elem, _Traits >& os, _Graph const& g)
	{
		typedef boost::property_map< _Graph, boost::vertex_index_t >::type index_map_t;
		index_map_t index = boost::get(boost::vertex_index, g);

		typedef graph_traits< _Graph >::vertex_descriptor g_node;
		typedef graph_traits< _Graph >::edge_descriptor g_edge;
		typedef graph_traits< _Graph >::vertex_iterator g_node_it;
		typedef graph_traits< _Graph >::out_edge_iterator g_out_edge_it;

		g_node_it v_it, v_end;
		for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
		{
			g_node u = *v_it;

			os << index[u] <<  ": ";

			g_out_edge_it oe_it, oe_end;
			for(boost::tie(oe_it, oe_end) = boost::out_edges(u, g); oe_it != oe_end; ++oe_it)
			{
				g_edge e = *oe_it;
				g_node v = boost::target(e, g);

				os << index[v] << " ";
			}

			os << std::endl;
		}

		return os;
	}

}


#endif

