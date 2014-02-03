// direct_junctions.cpp

#include "direct_junctions.h"
#include "iterative_junction_gen.h"


namespace iter_junc {

	// TODO:!!!!!!!!!!!!!!!!!!!!!!!
	float_t const ExternalTangentFactor = 0.4f;
	float_t const InternalTangentFactor = 0.125f;

	void direct_junctions::generate_initial_junctions(node_junction_map& junctions, graph_data const& gd)
	{
		/*
		Create interfaces both positioned and with tangents along straight lines between connected nodes.
		*/
		gk::g_edge_it e_it, e_end;
		for(boost::tie(e_it, e_end) = boost::edges(gd.g); e_it != e_end; ++e_it)
		{
			reflect_new_edge(junctions, *e_it, gd);
		}

		// Create pseudo interfaces for start and terminal nodes
		gk::g_node_it v_it, v_end;
		for(boost::tie(v_it, v_end) = boost::vertices(gd.g); v_it != v_end; ++v_it)
		{
			gk::g_node v = *v_it;

			if(boost::in_degree(v, gd.g) == 0)
			{
				junctions[v].in_interfaces[boost::none].int_point_vec.setZero();
				junctions[v].in_interfaces[boost::none].int_tangent = vector_t(0, 0.001f, 0);
					//.setZero();	// TODO: Is this allowed?

					// Seems it's ok, but can lead to problems, eg. normal generation for mesh.
					// But even if don't explicitly make zero tangent here, can in theory occur at some point on curves
					// so perhaps should use approximation methods for such situations (ie. v[t] = p[t+delta] - p[t])

					// And anyway since tangents dont need to be synced in these cases (start node will have only one out edge,
					// and terminal nodes (below) can have multiple in edges all converging on the center point with any tangent),
					// maybe we should forget pseudo interfaces, and just create tangents as needed when creating spline accessor.
			}
			else if(boost::out_degree(v, gd.g) == 0)
			{
				junctions[v].out_interfaces[boost::none].int_point_vec.setZero();
				junctions[v].out_interfaces[boost::none].int_tangent = vector_t(0, 0.001f, 0);
					//.setZero();
			}
		}
	}

	void direct_junctions::reflect_new_edge(node_junction_map& junctions, gk::g_edge e_new, graph_data const& gd)
	{
		gk::g_node u = boost::source(e_new, gd.g);
		gk::g_node v = boost::target(e_new, gd.g);

		// Straight line vector from source to target nodes
		vector_t uv = (gd.node_positions.at(v) - gd.node_positions.at(u)).cast< float_t >();
		vector_t uv_dir = uv.normalized();

		junctions[u].out_interfaces[e_new].int_point_vec = uv_dir;
		junctions[u].out_interfaces[e_new].int_tangent = uv_dir;// uv * TangentFactor;	// TODO: length... maybe leaving it the length of
		// (or proportional to) unnormalized uv isnt a bad idea

		junctions[v].in_interfaces[e_new].int_point_vec = -uv_dir;
		junctions[v].in_interfaces[e_new].int_tangent = uv_dir;// uv * TangentFactor;
	}
	
	void direct_junctions::reflect_removed_edge(node_junction_map& junctions, gk::g_edge e_removed, graph_data const& gd)
	{
		gk::g_node u = boost::source(e_removed, gd.g);
		gk::g_node v = boost::target(e_removed, gd.g);

		junctions[u].out_interfaces.erase(e_removed);
		junctions[v].in_interfaces.erase(e_removed);

		// TODO: Modify pseudo junctions if this removal will result in u becoming terminal, or v having no more in edges
		// Kind of hassle, thinking better to removed pseudo interfaces entirely, data only needed at spline gen stage anyway
	}
	
	vector_t direct_junctions::junction_average_in_location(node_junction_data const& jd, graph_data const& gd)
	{
		vector_t avg = vector_t::Zero();

		for(auto const& in_if: jd.in_interfaces)
		{
			if(in_if.first != boost::none)
			{
				// Sum the unit vectors
				avg += in_if.second.int_point_vec;
			}
		}

		// Then normalize
		return avg.normalized();
	}

	vector_t direct_junctions::junction_average_out_location(node_junction_data const& jd, graph_data const& gd)
	{
		vector_t avg = vector_t::Zero();

		for(auto const& out_if: jd.out_interfaces)
		{
			if(out_if.first != boost::none)
			{
				// Sum the unit vectors
				avg += out_if.second.int_point_vec;
			}
		}

		// Then normalize
		return avg.normalized();
	}

/*
	float_t direct_junctions::evaluate_in_interface(node_junction_data const& jd, gk::g_edge in_edge, graph_data const& gd)
	{
		return 0.0f;
	}

	float_t direct_junctions::evaluate_out_interface(node_junction_data const& jd, gk::g_edge out_edge, graph_data const& gd)
	{
		return 0.0f;
	}
*/
	direct_junctions::in_edge_interface direct_junctions::ideal_in_interface(node_junction_data const& jd, gk::g_edge in_edge, graph_data const& gd)
	{
		gk::g_node n = boost::target(in_edge, gd.g);
		bool has_out_edges = boost::out_degree(n, gd.g) != 0;
		vector_t avg_out = has_out_edges ? junction_average_out_location(jd, gd) : vector_t::Zero();

		gk::g_node src = boost::source(in_edge, gd.g);
		vector_t back_to_source = (gd.node_positions.at(src) - gd.node_positions.at(n)).cast< float_t >();
		float_t edge_len = back_to_source.norm();
		back_to_source /= edge_len;

		in_edge_interface ideal;
		vector_t combination = -avg_out + back_to_source;
		float_t mag = combination.norm();
		if(mag == (float_t)0.0)
		{
			// Unlikely special case, just use existing interface location
			ideal.int_point_vec = jd.in_interfaces.at(in_edge).int_point_vec;
		}
		else
		{
			// Use a linear combination
			ideal.int_point_vec = combination / mag;
		}

		ideal.int_tangent = -ideal.int_point_vec;// * edge_len * TangentFactor;
		return ideal;
	}

	direct_junctions::out_edge_interface direct_junctions::ideal_out_interface(node_junction_data const& jd, gk::g_edge out_edge, graph_data const& gd)
	{
		gk::g_node n = boost::source(out_edge, gd.g);
		bool has_in_edges = boost::in_degree(n, gd.g) != 0;
		vector_t avg_in = has_in_edges ? junction_average_in_location(jd, gd) : vector_t::Zero();

		gk::g_node dst = boost::target(out_edge, gd.g);
		vector_t to_dest = (gd.node_positions.at(dst) - gd.node_positions.at(n)).cast< float_t >();
		float_t edge_len = to_dest.norm();
		to_dest /= edge_len;

		out_edge_interface ideal;
		vector_t combination = -avg_in + to_dest;
		float_t mag = combination.norm();
		if(mag == (float_t)0.0)
		{
			// Unlikely special case, just use existing interface location
			ideal.int_point_vec = jd.out_interfaces.at(out_edge).int_point_vec;
		}
		else
		{
			// Use a linear combination
			ideal.int_point_vec = combination / mag;
		}

		ideal.int_tangent = ideal.int_point_vec;// * edge_len * TangentFactor;
		return ideal;
	}

	direct_junctions::spline_accessor direct_junctions::create_spline_accessor(node_junction_map const& junctions, graph_data const& gd)
	{
		float_t const NodeRadius = 0.04f;	// TODO: !!!!!!!!!!!!!!!!

		spline_accessor acc;

		// Loop through all node junctions
		for(auto const& elem: junctions)
		{
			gk::g_node n = elem.first;
			node_junction_data const& jd = elem.second;

			// First do internal splines
			for(auto const& in: jd.in_interfaces)
			{
				for(auto const& out: jd.out_interfaces)
				{
					std::array< vector_t, 2 > points = {
						gd.node_positions.at(n).cast< float_t >() + in.second.int_point_vec * NodeRadius,
						gd.node_positions.at(n).cast< float_t >() + out.second.int_point_vec * NodeRadius
					};

					std::array< vector_t, 2 > tangents = {
						in.second.int_tangent * InternalTangentFactor,
						out.second.int_tangent * InternalTangentFactor
					};

					cubic_spline& s = acc.splines[spline_accessor::locator(true, in.first, out.first)];
					s.set_control_points(2, points.begin(), tangents.begin());
				}
			}

			// Now do any outgoing connection splines
			gk::g_out_edge_it e_it, e_end;
			for(boost::tie(e_it, e_end) = boost::out_edges(n, gd.g); e_it != e_end; ++e_it)
			{
				gk::g_edge e = *e_it;
				gk::g_node v = boost::target(e, gd.g);

				std::array< vector_t, 2 > points = {
					gd.node_positions.at(n).cast< float_t >() + jd.out_interfaces.at(e).int_point_vec * NodeRadius,
					gd.node_positions.at(v).cast< float_t >() + junctions.at(v).in_interfaces.at(e).int_point_vec * NodeRadius
				};

				std::array< vector_t, 2 > tangents = {
					jd.out_interfaces.at(e).int_tangent * ExternalTangentFactor,
					junctions.at(v).in_interfaces.at(e).int_tangent * ExternalTangentFactor
				};

				cubic_spline& s = acc.splines[spline_accessor::locator(false, e, boost::none)];
				s.set_control_points(2, points.begin(), tangents.begin());
			}
		}

		return acc;
	}

}




