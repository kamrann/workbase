// gen_spatial.h

#ifndef __GEN_SPATIAL_H
#define __GEN_SPATIAL_H

#include "graph_defs/graph_defn.h"
#include "util/proportional_selection.h"

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <tuple>
#include <vector>
#include <map>
#include <chrono>


namespace gk {
namespace ggen {

	typedef std::vector< Eigen::Vector2d, Eigen::aligned_allocator< Eigen::Vector2d > > node_coords_2d;
	typedef std::vector< Eigen::Vector3d, Eigen::aligned_allocator< Eigen::Vector3d > > node_coords_3d;

	// Generates randomly distributed nodes in a rectangular space
	template <
		typename RGen
	>
	inline void generate_spatial_nodes(
		node_coords_3d& coords,			// Output parameter for generated node coordinates
		RGen& rgen,
		double length_factor = 1.0,		// A value of 1.0 results in distribution within a unit square
		double density = 1.0			// Number of nodes per unit area, excluding start and finish nodes
		)
	{
		bool in3d = true;

		double const width = 1.0;
		double const length = length_factor * width;
		double const area = length * width;

		// TODO: temp, currently potentially incompatible with density parameter
		double const min_distance = 0.25;

		// Todo: for x, probably want normal dist or similar
		boost::random::uniform_real_distribution<> x_dist(-width / 2, width / 2);
		boost::random::uniform_real_distribution<> y_dist(0.0, length);
		boost::random::uniform_real_distribution<> theta_dist(0.0, boost::math::double_constants::pi);

		size_t const num_medial_nodes = (size_t)boost::math::iround(density * area);
		coords.resize(2 + num_medial_nodes);
		size_t i_node = 0;

		// Generate start node
		coords[i_node++] = Eigen::Vector3d(0.0, 0.0, 0.0);

		// Generate finish node
		coords[i_node++] = Eigen::Vector3d(0.0, length, 0.0);

		// Generate randomly distributed nodes
		for(size_t i = 0; i < num_medial_nodes; ++i)
		{
			bool okay;
			do
			{
				coords[i_node] = Eigen::Vector3d(
					x_dist(rgen),
					y_dist(rgen),
					0
					);

				if(in3d)
				{
					double theta = theta_dist(rgen);
					coords[i_node] = Eigen::AngleAxis< double >(theta, Eigen::Vector3d(0, 1, 0)) * coords[i_node];
				}

				okay = true;
				for(size_t j = 0; j < i_node; ++j)
				{
					if((coords[i_node] - coords[j]).norm() < min_distance)
					{
						okay = false;
						break;
					}
				}
			}
			while(!okay);

			++i_node;
		}

		// Put finish node last
		std::swap(coords[1], coords[1 + num_medial_nodes]);
	}

	namespace detail {

		struct node_counter: public boost::default_bfs_visitor
		{
			size_t& count;

			node_counter(size_t& _c): count(_c)
			{
				count = 0;
			}

			inline void discover_vertex(g_node u, graph const& g)
			{
				++count;
			}
		};

		struct reachable: public boost::default_bfs_visitor
		{
			g_node n;
			bool& found;

			reachable(g_node _n, bool& _f): n(_n), found(_f)
			{
				found = false;
			}

			inline void discover_vertex(g_node u, graph const& g)
			{
				if(u == n)
				{
					found = true;
				}
			}
		};

		typedef std::map< g_node, double > node_viability_map;
		typedef std::map< std::pair< g_node, g_node >, double > edge_viability_map;
			
		inline double edge_viability(
			graph const& g,
			g_node u,
			g_node v,
			node_coord_map_3d& cd_map,
			double const max_dist = 0.5
			)
		{
			// No duplicate edges
			if(boost::edge(u, v, g).second)
			{
				return 0.0;
			}

			// No cycles for now
			bool is_reachable = false;
			boost::breadth_first_search(g, v, boost::visitor(detail::reachable(u, is_reachable)));
			if(is_reachable)
			{
				return 0.0;
			}

			auto u_cd = cd_map[u];
			auto v_cd = cd_map[v];
			auto rel_vec = v_cd - u_cd;

			// Distance from u to v
			double const distance = rel_vec.norm();
			double const dist_factor = 2.0 * (1.0 - 1.0 / (1.0 + std::pow(boost::math::double_constants::e, -4.0 * (distance / max_dist))));

			// Cosine of angle between uv and graph forward direction
			double const cos_angle = rel_vec.dot(Eigen::Vector3d(0, 1.0, 0)) / distance;
			double angle_factor = (cos_angle + 1.0) / 2.0;

			return dist_factor * angle_factor;

			// TODO: Edge uv should be more viable if u has an incident edge vector with small angle to uv.
			// ie. edge uv would be in a way continuing the flow direction of one (or the average?) of edges incident to u.
		}

		template <
			typename PossibleSourceIterator,
			typename PossibleDestinationIterator,
			typename UnviableSources,
			typename RGen
		>
		inline std::tuple< g_node, g_node, bool > choose_edge_to_insert__random_source(
			graph const& g,
			node_coord_map_3d& coord_map,
			PossibleSourceIterator poss_src_first,
			PossibleSourceIterator poss_src_last,
			PossibleDestinationIterator poss_dst_first,
			PossibleDestinationIterator poss_dst_last,
			RGen& rgen,
			UnviableSources& unviable_srcs
			)
		{
			typedef PossibleSourceIterator poss_src_it_t;
			typedef PossibleDestinationIterator poss_dst_it_t;

			size_t const num_possible_sources = poss_src_last - poss_src_first;

			// Randomly pick a starting node u from the active nodes
			g_node u = poss_src_first[
				boost::random::uniform_int_distribution<>(0, num_possible_sources - 1)(rgen)];

			// Want to construct a viability map of all the possible destination nodes
			node_viability_map viability_map;
			double total_viability = 0.0;

			// Iterate over all possible destination nodes
			for(poss_dst_it_t it_dst = poss_dst_first; it_dst != poss_dst_last; ++it_dst)
			{
				g_node poss_v = *it_dst;

				// No loops for now
				if(poss_v == u)
				{
					continue;
				}

				double const viability_uv = detail::edge_viability(g, u, poss_v, coord_map, 0.5);// TODO: width / 2.0);
				if(viability_uv > 0.0)
				{
					viability_map[poss_v] = viability_uv;
					total_viability += viability_uv;
				}
			}

			if(viability_map.empty())
			{
				// Mark poss_u as an unviable source node
				unviable_srcs.insert(u);
				return std::make_tuple(g_node(), g_node(), false);
			}

			// Now choose edge randomly but proportionally to viability
			struct deref: public std::unary_function< detail::node_viability_map::value_type const&, double >
			{
				inline double operator() (detail::node_viability_map::value_type const& val) const
				{
					return val.second;
				}
			};
			
			typedef boost::transform_iterator< deref, detail::node_viability_map::const_iterator > viab_it;

			double const rand_point = boost::random::uniform_real_distribution<>(0.0, total_viability)(rgen);
			viab_it it_selected = select_proportionally(viab_it(viability_map.begin()), viab_it(viability_map.end()), rand_point);
			assert(it_selected != viab_it(viability_map.end()));

			return std::make_tuple(u, it_selected.base()->first, true);
		}

		template <
			typename PossibleSourceIterator,
			typename PossibleDestinationIterator,
			typename UnviableNodes,
			typename RGen
		>
		inline std::tuple< g_node, g_node, bool > choose_edge_to_insert__edge_viability(
			graph const& g,
			node_coord_map_3d& coord_map,
			PossibleSourceIterator poss_src_first,
			PossibleSourceIterator poss_src_last,
			PossibleDestinationIterator poss_dst_first,
			PossibleDestinationIterator poss_dst_last,
			RGen& rgen,
			UnviableNodes& unviable_srcs,
			UnviableNodes& unviable_dsts
			)
		{
			typedef PossibleSourceIterator poss_src_it_t;
			typedef PossibleDestinationIterator poss_dst_it_t;

			// Want to construct a viability map of all the possible edges
			edge_viability_map viability_map;
			double total_viability = 0.0;

			unviable_dsts.clear();
			unviable_dsts.insert(poss_dst_first, poss_dst_last);

			// Iterate over all possible source nodes
			for(poss_src_it_t it_src = poss_src_first; it_src != poss_src_last; ++it_src)
			{
				g_node poss_u = *it_src;
				bool viable_src = false;

				// Iterate over all possible destination nodes
				for(poss_dst_it_t it_dst = poss_dst_first; it_dst != poss_dst_last; ++it_dst)
				{
					g_node poss_v = *it_dst;

					// No loops for now
					if(poss_v == poss_u)
					{
						continue;
					}

					double const viability_uv = detail::edge_viability(g, poss_u, poss_v, coord_map, 0.5);// TODO: width / 2.0);
					if(viability_uv > 0.0)
					{
						viability_map[std::make_pair(poss_u, poss_v)] = viability_uv;
						total_viability += viability_uv;
						viable_src = true;
						unviable_dsts.erase(poss_v);
					}
				}

				if(!viable_src)
				{
					// Mark poss_u as an unviable source node
					unviable_srcs.insert(poss_u);
				}
			}

			if(viability_map.empty())
			{
				return std::make_tuple(g_node(), g_node(), false);
			}

			// Now choose edge randomly but proportionally to viability
			struct deref: public std::unary_function< detail::edge_viability_map::value_type const&, double >
			{
				inline double operator() (detail::edge_viability_map::value_type const& val) const
				{
					return val.second;
				}
			};
			
			typedef boost::transform_iterator< deref, detail::edge_viability_map::const_iterator > viab_it;

			double const rand_point = boost::random::uniform_real_distribution<>(0.0, total_viability)(rgen);
			viab_it it_selected = select_proportionally(viab_it(viability_map.begin()), viab_it(viability_map.end()), rand_point);
			assert(it_selected != viab_it(viability_map.end()));

			return std::make_tuple(it_selected.base()->first.first, it_selected.base()->first.second, true);
		}

	}

	// TODO: Too much duplication in following functions...
	// Maybe make use of meta-dependencies??
	struct iterative_gen_data
	{
		node_coord_map_3d							coord_map;
		std::vector< g_node >						active_nodes;
		std::map< size_t, std::vector< g_node > >	out_degree_map, in_degree_map;
	};

	template <
		typename RGen
	>
	inline void generate_unconnected_graph_from_spatial_nodes(
		node_coords_3d const& coords,
		graph& g,
		iterative_gen_data& igdata,
		RGen& rgen
		)
	{
		// Insert all nodes into the graph, maintaining a map to their spatial coordinates
		g.clear();
		for(auto& cd: coords)
		{
			g_node u = boost::add_vertex(g);
			igdata.coord_map[u] = cd;
		}

		g_node start = boost::vertex(0, g);
		igdata.active_nodes.push_back(start);
		igdata.out_degree_map[0].push_back(start);
	}

	template <
		typename RGen
	>
	inline std::tuple<
		bool,		// Edge could be added?
		g_edge,		// Descriptor for added edge
		bool		// Graph is connected?
	>
	add_graph_edge_spatial_nodes(
		graph& g,
		iterative_gen_data& igdata,
		RGen& rgen
		)
	{
		// Start node
		g_node start = boost::vertex(0, g);

		size_t const num_nodes = boost::num_vertices(g);
		while(!igdata.active_nodes.empty())
		{
			// For now, only active nodes with the smallest current out-degree are considered as sources
			size_t deg = 0;
			while(igdata.out_degree_map[deg].empty())
			{
				++deg;
			}

			// Choose the edge to insert
			g_node_it v_it, v_end;
			std::tie(v_it, v_end) = boost::vertices(g);
			g_node u, v;
			bool success;
			std::set< g_node > unviable_srcs;
			//			std::tie(u, v, success) = detail::choose_edge_to_insert__random_source(
			std::tie(u, v, success) = detail::choose_edge_to_insert__edge_viability(
				g,
				igdata.coord_map,
				igdata.out_degree_map[deg].begin(),
				igdata.out_degree_map[deg].end(),
				v_it,
				v_end,
				rgen,
				unviable_srcs);

			// Mark nodes which are no longer viable sources
			for(g_node n: unviable_srcs)
			{
				// Mark as inactive
				igdata.active_nodes.erase(std::find(igdata.active_nodes.begin(), igdata.active_nodes.end(), n));
				size_t deg = boost::out_degree(n, g);
				igdata.out_degree_map[deg].erase(std::find(igdata.out_degree_map[deg].begin(), igdata.out_degree_map[deg].end(), n));
			}

			// If we failed to find an edge to add, try again (or bail if no more viable sources)
			if(!success)
			{
				continue;
			}

			// So, we found a viable edge, uv, to insert...

			// If v was not previously connected (and is not finish node), add to active nodes
			if(boost::in_degree(v, g) == 0 && v != boost::vertex(num_nodes - 1, g))
			{
				igdata.active_nodes.push_back(v);
				igdata.out_degree_map[0].push_back(v);
			}

			// Add the edge uv to our graph
			auto res = boost::add_edge(u, v, g);
			assert(res.second);
			g_edge e_new = res.first;

			// Update out degree mapping
			deg = boost::out_degree(u, g);
			igdata.out_degree_map[deg - 1].erase(std::find(igdata.out_degree_map[deg - 1].begin(), igdata.out_degree_map[deg - 1].end(), u));
			igdata.out_degree_map[deg].push_back(u);

			// Test for connectivity
			size_t visit_count = 0;
			boost::breadth_first_search(g, start, boost::visitor(detail::node_counter(visit_count)));
			bool connected = (visit_count == num_nodes);

			return std::make_tuple(true, e_new, connected);
		}

		// Test for connectivity
		size_t visit_count = 0;
		boost::breadth_first_search(g, start, boost::visitor(detail::node_counter(visit_count)));
		bool connected = (visit_count == num_nodes);

		return std::make_tuple(false, g_edge(), connected);
	}

	template <
		typename RGen
	>
	inline void generate_unconnected_graph_from_spatial_nodes2(
		node_coords_3d const& coords,
		graph& g,
		iterative_gen_data& igdata,
		RGen& rgen
		)
	{
		// Insert all nodes into the graph, maintaining a map to their spatial coordinates
		g.clear();
		for(auto& cd: coords)
		{
			g_node u = boost::add_vertex(g);
			igdata.coord_map[u] = cd;
		}

		g_node start = boost::vertex(0, g);
		g_node_it v_it, v_end;
		boost::tie(v_it, v_end) = boost::vertices(g);
		++v_it;	// Start node not potential edge destination
		igdata.active_nodes.insert(igdata.active_nodes.begin(), v_it, v_end);
		//igdata.out_degree_map[0].push_back(start);
		igdata.in_degree_map[0] = igdata.active_nodes;
	}

	template <
		typename RGen
	>
	inline std::tuple<
		bool,		// Edge could be added?
		g_edge,		// Descriptor for added edge
		bool		// Graph is connected?
	>
	add_graph_edge_spatial_nodes2(
		graph& g,
		iterative_gen_data& igdata,
		RGen& rgen
		)
	{
		// Start node
		g_node start = boost::vertex(0, g);

		size_t const num_nodes = boost::num_vertices(g);
		while(!igdata.active_nodes.empty())
		{
			// For now, only active nodes with the smallest current in-degree are considered as sources
			size_t deg = 0;
			while(igdata.in_degree_map[deg].empty())
			{
				++deg;
			}

			// Choose the edge to insert
			g_node_it v_it, v_end;
			std::tie(v_it, v_end) = boost::vertices(g);
			--v_end;	// Finish node cannot be edge source
			g_node u, v;
			bool success;
			std::set< g_node > unviable_srcs, unviable_dsts;
			//			std::tie(u, v, success) = detail::choose_edge_to_insert__random_source(
			std::tie(u, v, success) = detail::choose_edge_to_insert__edge_viability(
				g,
				igdata.coord_map,
				v_it,
				v_end,
				igdata.in_degree_map[deg].begin(),
				igdata.in_degree_map[deg].end(),
				rgen,
				unviable_srcs,
				unviable_dsts);

			// Mark nodes which are no longer viable destinations
			for(g_node n: unviable_dsts)
			{
				// Mark as inactive
				igdata.active_nodes.erase(std::find(igdata.active_nodes.begin(), igdata.active_nodes.end(), n));
				size_t deg = boost::in_degree(n, g);
				igdata.in_degree_map[deg].erase(std::find(igdata.in_degree_map[deg].begin(), igdata.in_degree_map[deg].end(), n));
			}

			// If we failed to find an edge to add, try again (or bail if no more viable destinations)
			if(!success)
			{
				continue;
			}

			// So, we found a viable edge, uv, to insert...

/*			// If v was not previously connected (and is not finish node), add to active nodes
			if(boost::in_degree(v, g) == 0 && v != boost::vertex(num_nodes - 1, g))
			{
				igdata.active_nodes.push_back(v);
				igdata.out_degree_map[0].push_back(v);
			}
*/
			// Add the edge uv to our graph
			auto res = boost::add_edge(u, v, g);
			assert(res.second);
			g_edge e_new = res.first;

			// Update in-degree mapping
			deg = boost::in_degree(v, g);
			igdata.in_degree_map[deg - 1].erase(std::find(igdata.in_degree_map[deg - 1].begin(), igdata.in_degree_map[deg - 1].end(), v));
			igdata.in_degree_map[deg].push_back(v);

			// Test for connectivity
			size_t visit_count = 0;
			boost::breadth_first_search(g, start, boost::visitor(detail::node_counter(visit_count)));
			bool connected = (visit_count == num_nodes);

			return std::make_tuple(true, e_new, connected);
		}

		// Test for connectivity
		size_t visit_count = 0;
		boost::breadth_first_search(g, start, boost::visitor(detail::node_counter(visit_count)));
		bool connected = (visit_count == num_nodes);

		return std::make_tuple(false, g_edge(), connected);
	}

	template <
		typename RGen
	>
	inline void generate_graph_from_spatial_nodes(
		node_coords_3d const& coords,
		graph& g,
		RGen& rgen
		)
	{
		// Insert all nodes into the graph, maintaining a map to their spatial coordinates
		g.clear();
		node_coord_map_3d coord_map;
		for(auto& cd: coords)
		{
			g_node u = boost::add_vertex(g);
			coord_map[u] = cd;
		}

		// Start node
		g_node start = boost::vertex(0, g);

		// Retain set of active nodes to which we can add out-edges.
		// Initialise it to the start node, and add nodes once they are connected.
		// Remove nodes when there are no longer any viable out-edges to add to them.
		std::vector< g_node > active_nodes;
		active_nodes.push_back(start);

		// Also keep track of active nodes by their out-degree
		std::map< size_t, std::vector< g_node > > out_degree_map;
		out_degree_map[0].push_back(start);

		size_t const num_nodes = boost::num_vertices(g);
		bool connected = false;
		while(!connected && !active_nodes.empty())
		{
			// For now, only active nodes with the smallest current out-degree are considered as sources
			size_t deg = 0;
			while(out_degree_map[deg].empty())
			{
				++deg;
			}

			// Choose the edge to insert
			g_node_it v_it, v_end;
			std::tie(v_it, v_end) = boost::vertices(g);
			g_node u, v;
			bool success;
			std::set< g_node > unviable_srcs;
//			std::tie(u, v, success) = detail::choose_edge_to_insert__random_source(
			std::tie(u, v, success) = detail::choose_edge_to_insert__edge_viability(
				g,
				coord_map,
				out_degree_map[deg].begin(),
				out_degree_map[deg].end(),
				v_it,
				v_end,
				rgen,
				unviable_srcs);
			
			// Mark nodes which are no longer viable sources
			for(g_node n: unviable_srcs)
			{
				// Mark as inactive
				active_nodes.erase(std::find(active_nodes.begin(), active_nodes.end(), n));
				size_t deg = boost::out_degree(n, g);
				out_degree_map[deg].erase(std::find(out_degree_map[deg].begin(), out_degree_map[deg].end(), n));
			}

			// If we failed to find an edge to add, try again (or bail if no more viable sources)
			if(!success)
			{
				continue;
			}

			// So, we found a viable edge, uv, to insert...

			// If v was not previously connected (and is not finish node), add to active nodes
			if(boost::in_degree(v, g) == 0 && v != boost::vertex(num_nodes - 1, g))
			{
				active_nodes.push_back(v);
				out_degree_map[0].push_back(v);
			}

			// Add the edge uv to our graph
			boost::add_edge(u, v, g);

			// Update out degree mapping
			deg = boost::out_degree(u, g);
			out_degree_map[deg - 1].erase(std::find(out_degree_map[deg - 1].begin(), out_degree_map[deg - 1].end(), u));
			out_degree_map[deg].push_back(u);

			// Test for connectivity
			size_t visit_count = 0;
			boost::breadth_first_search(g, start, boost::visitor(detail::node_counter(visit_count)));
			connected = (visit_count == num_nodes);
		}
	}

}
}


#endif


