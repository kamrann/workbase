// level_graph.cpp

#include "level_graph.h"
#include "agent.h"
#include "iterative_junction_gen.h"

#include "util/graphics3d/random.h"
#include "util/cubic_spline.h"

#include <boost/optional.hpp>


level_graph::level_graph()
{

}

bool level_graph::generate_graph(random_gen_t& rgen)
{
	double const LevelLength = 4.0;
	double const Density = 5.0;
	gk::ggen::node_coords_3d coords;
	gk::ggen::generate_spatial_nodes(coords, rgen, LevelLength, Density);
	gk::ggen::generate_unconnected_graph_from_spatial_nodes2(coords, g, gen_data, rgen);

	for(size_t i = 0; i < coords.size(); ++i)
	{
		gk::g_node u = boost::vertex(i, g);
		node_locations[u] = coords[i];
	}

	bool graph_connected = false;

	while(!graph_connected)
	{
		bool added;
		gk::g_edge edge;
		std::tie(added, edge, graph_connected) = gk::ggen::add_graph_edge_spatial_nodes2(g, gen_data, rgen);

		if(added)
		{
			std::cout << "Successfully added edge" << std::endl;

			if(graph_connected)
			{
				std::cout << "Graph now connected" << std::endl;
			}
		}
		else
		{
			std::cout << "Unable to add anymore edges" << std::endl;
		}
	}

	generate_junctions(rgen);

	return true;
}

void level_graph::increment_graph(random_gen_t& rgen)
{
	bool added, connected;
	gk::g_edge edge;
	std::tie(added, edge, connected) = gk::ggen::add_graph_edge_spatial_nodes2(g, gen_data, rgen);

	if(added)
	{
		iter_junc::graph_data gd(g, node_locations);
		iter_junc::direct_junctions().reflect_new_edge(junctions, edge, gd);

		splines = iter_junc::direct_junctions().create_spline_accessor(junctions, gd);
	
		std::cout << "Successfully added edge" << std::endl;
		if(connected)
		{
			std::cout << "Graph now connected" << std::endl;
		}
	}
	else
	{
		std::cout << "Unable to add anymore edges" << std::endl;
	}
}

void level_graph::generate_junctions(random_gen_t& rgen)
{
	junctions = iter_junc::direct_junctions::node_junction_map();
	iter_junc::graph_data gd(g, node_locations);
	iter_junc::direct_junctions().generate_initial_junctions(junctions, gd);

	splines = iter_junc::direct_junctions().create_spline_accessor(junctions, gd);
}

void level_graph::incremental_junction_update()
{
	iter_junc::graph_data gd(g, node_locations);
	gk::g_node n = boost::vertex(junc_iter_data.node_index, g);

	for(auto& in_if: junctions[n].in_interfaces)
	{
		if(in_if.first)
		{
			iter_junc::direct_junctions::in_edge_interface ideal = iter_junc::direct_junctions().ideal_in_interface(
				junctions[n],
				*in_if.first,
				gd);
			in_if.second = ideal;
		}
	}

	for(auto& out_if: junctions[n].out_interfaces)
	{
		if(out_if.first)
		{
			iter_junc::direct_junctions::out_edge_interface ideal = iter_junc::direct_junctions().ideal_out_interface(
				junctions[n],
				*out_if.first,
				gd);
			out_if.second = ideal;
		}
	}

	splines = iter_junc::direct_junctions().create_spline_accessor(junctions, gd);

	++junc_iter_data.node_index;
	junc_iter_data.node_index %= boost::num_vertices(g);
}

cubic_spline level_graph::get_agent_spline(agent const& a) const
{
	return splines.get(a.at_junction, a.e1, a.e2);
}

Eigen::Vector3f level_graph::get_agent_position(agent const& a) const
{
	return get_agent_spline(a).at(a.pos);
}

Eigen::Vector3f level_graph::get_agent_direction(agent const& a) const
{
	return get_agent_spline(a).direction_at(a.pos);
}

bool level_graph::move_agent_to_connection(agent& a)
{
	assert(a.at_junction);

	if(!a.e2)
	{
		// Junction is terminal node
		return false;
	}

	Eigen::Vector3f initial_dir = get_agent_spline(a).direction_at(a.pos);
	a.e1 = a.e2;
	a.at_junction = false;
	a.e2 = boost::none;
	a.pos = 0;
	Eigen::Vector3f new_dir = get_agent_spline(a).direction_at(a.pos);
	a.up = Eigen::Quaternionf::FromTwoVectors(initial_dir, new_dir) * a.up;
	return true;
}

std::pair< bool, bool > level_graph::move_agent(agent& a, float& distance)
{
	while(distance > 0)
	{
		cubic_spline spl = get_agent_spline(a);
		float rem_length = spl.length(a.pos);
		if(distance >= rem_length)
		{
			if(a.at_junction)
			{
				if(!move_agent_to_connection(a))
				{
					return std::make_pair(true, false);
				}
				distance -= rem_length;
			}
			else
			{
				// On a connection and will reach junction
				Eigen::Vector3f initial_dir = get_agent_spline(a).direction_at(a.pos);
				a.pos = 1.0f;
				Eigen::Vector3f new_dir = get_agent_spline(a).direction_at(a.pos);
				a.up = Eigen::Quaternionf::FromTwoVectors(initial_dir, new_dir) * a.up;
				distance -= rem_length;
				return std::make_pair(false, true);
			}
		}
		else
		{
			Eigen::Vector3f initial_dir = get_agent_spline(a).direction_at(a.pos);
			a.pos = spl.t_at_distance(a.pos, distance);
			Eigen::Vector3f new_dir = get_agent_spline(a).direction_at(a.pos);
			a.up = Eigen::Quaternionf::FromTwoVectors(initial_dir, new_dir) * a.up;
			distance = 0;
		}
	}

	return std::make_pair(false, false);
}



