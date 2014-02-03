// level_graph.h

#ifndef __GK_LEVEL_GRAPH_H
#define __GK_LEVEL_GRAPH_H

#include "direct_junctions.h"

#include "graphgen/gen_spatial.h"


class agent;
class cubic_spline;

class level_graph
{
public:
	typedef boost::random::mt19937 random_gen_t;

public:
	gk::graph g;
	gk::node_coord_map_3d node_locations;
	gk::ggen::iterative_gen_data gen_data;

	/*
	struct crossover_junction
	{
		typedef std::map< gk::g_edge, std::pair< Eigen::Vector3f, Eigen::Vector3f > > edge_interface_map;

		// Entrance point on node surface and tangent, for every in edge
		edge_interface_map in_edge_interfaces;
		// Exit point on node surface and tangent, for every out edge
		edge_interface_map out_edge_interfaces;

		typedef std::pair< boost::optional< gk::g_edge >, boost::optional< gk::g_edge > > in_out_edge_pair;

		// Tangent at node center for each internal path
		// For start node, first element of pair is <none>
		// For terminal nodes, second element of pair is <none>
		typedef std::map<
			in_out_edge_pair,
			Eigen::Vector3f
		> center_map;

		center_map center_data;
	};

	std::map< gk::g_node, crossover_junction > node_junctions;
*/

	// TODO: This should be stored by an intermediate class in iterative_junction_gen.h
	iter_junc::direct_junctions::node_junction_map junctions;
	iter_junc::direct_junctions::iteration_data junc_iter_data;
	//

	iter_junc::direct_junctions::spline_accessor splines;

public:
	level_graph();

private:
	cubic_spline		get_agent_spline(agent const& a) const;

public:
	bool				generate_graph(random_gen_t& rgen);
	void				increment_graph(random_gen_t& rgen);

	void				generate_junctions(random_gen_t& rgen);
	void				incremental_junction_update();

	Eigen::Vector3f		get_agent_position(agent const& a) const;
	Eigen::Vector3f		get_agent_direction(agent const& a) const;

	// Puts agent currently at a junction at the start of the next connection spline and returns true; or, if junction is
	// terminal node, returns false.
	bool				move_agent_to_connection(agent& a);

	// Moves an agent by the given distance, or if a junction is reached within this distance, moves agent up to it and
	// subtracts the amount moved from distance.
	// result.first = true if agent reached a termination point.
	// result.second = true if agent reached a junction.
	std::pair< bool, bool >		move_agent(agent& a, float& distance);
};


#endif


