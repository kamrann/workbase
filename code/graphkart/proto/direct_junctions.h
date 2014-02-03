// direct_junctions.h

#ifndef __GK_DIRECT_JUNCTIONS_H
#define __GK_DIRECT_JUNCTIONS_H

#include "proto_types.h"

#include "graph_defs/graph_defn.h"

#include "util/cubic_spline.h"

#include <boost/optional.hpp>

#include <map>
#include <tuple>


namespace iter_junc {

	struct graph_data;

	// Defines a junction type where each internal spline has just 2 control points, the in/out interfaces,
	// and does not need to pass through the center point.
	class direct_junctions
	{
	public:
		typedef struct
		{
			vector_t int_point_vec;		// Unit vector from node center, specifying point on surface of the interface
			vector_t int_tangent;		// Tangent vector at the interface

		} in_edge_interface, out_edge_interface;

		// Interface for incoming edges, or in the case of boost::none, spline source at center of node
		typedef std::map< boost::optional< gk::g_edge >, in_edge_interface > in_interface_map;
		// Interface for outgoing edges, or in the case of boost::none, spline end at center of node
		typedef std::map< boost::optional< gk::g_edge >, out_edge_interface > out_interface_map;

		typedef struct {
			in_interface_map	in_interfaces;
			out_interface_map	out_interfaces;

		} node_junction_data;

		typedef std::map< gk::g_node, node_junction_data > node_junction_map;

		struct iteration_data
		{
			size_t node_index;

			iteration_data(): node_index(0)
			{}
		};

		class spline_accessor
		{
		private:
			typedef boost::optional< gk::g_edge > opt_edge;
			typedef std::tuple<
				bool,
				opt_edge,
				opt_edge
			> locator;

			std::map< locator, cubic_spline > splines;

		public:
			inline cubic_spline const& get(
				bool at_junction,
				opt_edge e1,
				opt_edge e2 = boost::none) const
			{
				return splines.at(locator(at_junction, e1, e2));
			}

			inline std::map< locator, cubic_spline >::const_iterator begin() const
			{
				return splines.begin();
			}

			inline std::map< locator, cubic_spline >::const_iterator end() const
			{
				return splines.end();
			}

		friend class direct_junctions;
		};

	private:
		vector_t junction_average_in_location(node_junction_data const& jd, graph_data const& gd);
		vector_t junction_average_out_location(node_junction_data const& jd, graph_data const& gd);

	public:
		void generate_initial_junctions(node_junction_map& junctions, graph_data const& gd);
		void reflect_new_edge(node_junction_map& junctions, gk::g_edge e_new, graph_data const& gd);
		void reflect_removed_edge(node_junction_map& junctions, gk::g_edge e_removed, graph_data const& gd);

		// Methods to give a score to an interface
//		float_t evaluate_in_interface(node_junction_data const& jd, gk::g_edge in_edge, graph_data const& gd);
//		float_t evaluate_out_interface(node_junction_data const& jd, gk::g_edge in_edge, graph_data const& gd);

		// Methods to generate the precise ideal interface
		in_edge_interface ideal_in_interface(node_junction_data const& jd, gk::g_edge in_edge, graph_data const& gd);
		out_edge_interface ideal_out_interface(node_junction_data const& jd, gk::g_edge out_edge, graph_data const& gd);

		spline_accessor create_spline_accessor(node_junction_map const& junctions, graph_data const& gd);
	};

}


#endif


