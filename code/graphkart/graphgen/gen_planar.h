// gen_planar.h

#ifndef __GEN_PLANAR_H
#define __GEN_PLANAR_H

#include "graph_defs/graph_defn.h"

#include <boost/graph/copy.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <map>


namespace gk {
namespace ggen {

	namespace detail {

		typedef std::map< g_node, size_t > shortest_path_length_map;
		typedef std::map< size_t, std::set< g_node > > level_map;

		// Assuming g is planar, can we add edge (uv) and maintain planarity?
		// TODO: Currently just do O(n) planarity test on modified graph, but clearly can be faster.
		// See google: iterative planarity test
		inline bool if_add_edge_still_planar(graph const& g, g_node u, g_node v)
		{
			typedef boost::property_map< graph, boost::vertex_index_t >::type index_map_t;
			typedef boost::iterator_property_map< typename std::vector< udg_node >::iterator,
				index_map_t, udg_node, udg_node& > iso_map_t;

			std::vector< udg_node > iso_vec(boost::num_vertices(g));
			iso_map_t v_map(iso_vec.begin(), boost::get(boost::vertex_index, g));
			ud_graph h;
			boost::copy_graph(g, h, boost::orig_to_copy(v_map));
			boost::add_edge(v_map[u], v_map[v], h);

			return boost::boyer_myrvold_planarity_test(h);
		}

		// Recalculate shortest paths from s to all nodes reachable from u (assume sp[u] is already updated)
		inline void update_shortest_paths(graph const& g, g_node s, g_node u, shortest_path_length_map& sp, level_map& lv)
		{
			std::vector< int > d_map(boost::num_vertices(g));
			boost::breadth_first_search(g, u,
				boost::visitor(
					boost::make_bfs_visitor(
						boost::record_distances(&d_map[0], boost::on_tree_edge())
						)));
			g_node_it v_it, v_end;
			for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
			{
				g_node n = *v_it;
				// TODO: Accessing d_map with a vertex_descriptor surely can't be right, although no doubt working given vecS...
				size_t sp_via_u = sp[u] + d_map[n];
				if(sp_via_u < sp[n])
				{
					lv[sp[n]].erase(n);
					sp[n] = sp_via_u;
					lv[sp[n]].insert(n);
				}
			}
		}

	}

	/* TODO:
	Generation by paths.
	
	Create a path of nodes S->F of length desired_length
	REPEAT
		Choose two nodes, X and Y (randomly but with criteria), and add a path of new nodes between them,
			of length roughly equal to length of XY.
	Probably need to add some single connections between existing nodes also.
	*/

	template <
		typename RGen
	>
	void generate_planar_graph(
		size_t desired_length,
		size_t desired_width,
		graph& g,
		RGen& rgen
		)
	{
		// Start with a single node
		g.clear();
		g_node start = boost::add_vertex(g);

		// Map to store the current shortest path from start to each node
		// TODO: May end up being more efficient to store the actual paths, rather than just the lengths
		detail::shortest_path_length_map shortest_paths;
		shortest_paths[start] = 0;

		// Also keep track of which nodes are at which level
		detail::level_map levels;
		levels[0].insert(start);

		// And a list of nodes which can still accommodate more out-edges
		std::vector< g_node > active_nodes(1, start);

		for(size_t i = 0; i < 20; ++i)
		{
			// Current 'length' of the graph
			size_t const num_levels = levels.size();

			// Choose source node from which to add an out edge
			// TODO: Random for now
			boost::random::uniform_int_distribution<> node_dist(0, active_nodes.size() - 1);
			g_node u = active_nodes[node_dist(rgen)];

			// Decide whether to link to a new node, or an existing one
			// First, determine set of potential existing nodes we could link to
			std::vector< g_node > potential_existing;
			g_node_it v_it, v_end;
			for(boost::tie(v_it, v_end) = boost::vertices(g); v_it != v_end; ++v_it)
			{
				g_node pot_v = *v_it;

				if(
					// No loops for now
					pot_v == u
					// No duplicate edges
					|| boost::edge(u, pot_v, g).second
					)
				{
					continue;
				}

				// TODO: Seems overkill to do this, just need to determine if u is reachable from v...
				std::vector< int > d_map(boost::num_vertices(g), -1);
				boost::breadth_first_search(g, pot_v,
					boost::visitor(
						boost::make_bfs_visitor(
							// distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));
							boost::record_distances(&d_map[0], boost::on_tree_edge())
							)));
				if(
					// No cycles for now, so we don't want to add (uv) if u is already reachable from v
					d_map[u] != -1
					)
				{
					continue;
				}

				if(
					// Don't add edge if g will lose planarity
					!detail::if_add_edge_still_planar(g, u, pot_v)
					)
				{
					continue;
				}

				potential_existing.push_back(pot_v);
			}

			// TODO:
			if(potential_existing.empty() || boost::random::uniform_int_distribution<>(0, 1)(rgen) == 0)
			{
				// Add new
				g_node v = boost::add_vertex(g);
				boost::add_edge(u, v, g);
				size_t sp = shortest_paths[u] + 1;
				shortest_paths[v] = sp;
				levels[sp].insert(v);
				active_nodes.push_back(v);
			}
			else
			{
				// Connect to existing
				// TODO:
				g_node v = potential_existing[boost::random::uniform_int_distribution<>(0, potential_existing.size() - 1)(rgen)];
				boost::add_edge(u, v, g);

				// Can v be reached via u on a path shorter than existing shortest path?
				if(shortest_paths[v] > shortest_paths[u] + 1)
				{
					// Update its shortest path and level
					levels[shortest_paths[v]].erase(v);
					shortest_paths[v] = shortest_paths[u] + 1;
					levels[shortest_paths[v]].insert(v);

					// And also all nodes reachable from v
					detail::update_shortest_paths(g, start, v, shortest_paths, levels);
				}
			}

			// Now decide if node u should remain active with regards to potentially adding more out-edges
			if(boost::random::uniform_int_distribution<>(0, 3)(rgen) == 0)
			{
				active_nodes.erase(std::find(active_nodes.begin(), active_nodes.end(), u));
			}
		}
	}

}
}


#endif


