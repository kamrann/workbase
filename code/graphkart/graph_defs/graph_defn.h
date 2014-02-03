// graph_defn.h

#ifndef __GRAPH_DEFN_H
#define __GRAPH_DEFN_H

#include <Eigen/Dense>

#include <boost/graph/adjacency_list.hpp>

#include <map>


namespace gk {

	typedef boost::vecS out_edge_list_sel_t;
	typedef boost::vecS vertex_list_sel_t;

	typedef boost::adjacency_list<
		out_edge_list_sel_t,
		vertex_list_sel_t,
		boost::bidirectionalS,
		boost::no_property,
		boost::property< boost::edge_index_t, int >		// Needed for some planar algorithms
	> graph;

/*	Maybe have to override the graph_traits if i do this ???
	class graph: public boost::adjacency_list<
		out_edge_list_sel_t,
		vertex_list_sel_t,
		boost::bidirectionalS,
		boost::no_property,
		boost::property< boost::edge_index_t, int >		// Needed for some planar algorithms
	>
	{};
*/
	typedef boost::graph_traits< graph >::vertex_descriptor		g_node;
	typedef boost::graph_traits< graph >::edge_descriptor		g_edge;
	typedef boost::graph_traits< graph >::vertex_iterator		g_node_it;
	typedef boost::graph_traits< graph >::edge_iterator			g_edge_it;
	typedef boost::graph_traits< graph >::out_edge_iterator		g_out_edge_it;
	typedef boost::graph_traits< graph >::in_edge_iterator		g_in_edge_it;


	typedef boost::adjacency_list<
		out_edge_list_sel_t,
		vertex_list_sel_t,
		boost::undirectedS,
		boost::no_property,
		boost::property< boost::edge_index_t, int >		// Needed for some planar algorithms
	> ud_graph;

	typedef boost::graph_traits< ud_graph >::vertex_descriptor		udg_node;
	typedef boost::graph_traits< ud_graph >::edge_descriptor		udg_edge;
	typedef boost::graph_traits< ud_graph >::vertex_iterator		udg_node_it;
	typedef boost::graph_traits< ud_graph >::edge_iterator			udg_edge_it;
	typedef boost::graph_traits< ud_graph >::out_edge_iterator		udg_out_edge_it;

	typedef std::map<
		g_node,
		Eigen::Vector2d,
		std::less< g_node >,
		Eigen::aligned_allocator< std::pair< const g_node, Eigen::Vector2d > >
	> node_coord_map_2d;
	typedef std::map<
		g_node,
		Eigen::Vector3d,
		std::less< g_node >,
		Eigen::aligned_allocator< std::pair< const g_node, Eigen::Vector3d > >
	> node_coord_map_3d;

}


#endif


