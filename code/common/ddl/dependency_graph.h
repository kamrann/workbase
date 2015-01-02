// dependency_graph.h

#ifndef __WB_DDL_DEPENDENCY_GRAPH_H
#define __WB_DDL_DEPENDENCY_GRAPH_H

#include "dependencies.h"
#include "sd_node_ref.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include <ostream>


namespace ddl {

	typedef node_ref defn_dg_vertex_props;	// think this should really be defn_node, not node_ref 
	typedef sd_node_ref sd_dg_vertex_props;

	enum class DependencyType {
		Generic,	// TODO: Perhaps useful to differentiate between
			// 1. Strong dependency - undefined if dep does not exist; and
			// 2. Weak dependency - no problem if dep doesn't exist, but must be notified of any change
		Parent,
	};
	
	struct dg_edge_props
	{
		DependencyType dep_type;
	};

	template < typename VertexProps >
	class dependency_graph:
		public boost::adjacency_list <
		boost::setS,
		boost::listS,	// vecS is probably faster, but is hassle due to invalidation of descriptors
		boost::bidirectionalS,//boost::directedS,
		VertexProps,
		dg_edge_props
		>
	{
	public:
		std::vector< vertex_descriptor > dependency_graph::topological_ordering() const
		{
			typedef std::map< vertex_descriptor, boost::default_color_type > color_map_t;
			color_map_t color_map_cont;
			boost::associative_property_map< color_map_t > color_map{ color_map_cont };
			std::vector< vertex_descriptor > output;
			boost::topological_sort(*this, std::back_inserter(output), boost::color_map(color_map));
			return output;
		}
	};

	typedef dependency_graph< defn_dg_vertex_props > defn_dep_graph;
	typedef dependency_graph< sd_dg_vertex_props > sd_dep_graph;


	defn_dep_graph build_defn_dependency_graph(defn_node hierarchy_root, defn_node dep_root);
	sd_dep_graph build_dependency_graph(sd_tree const& tree, sd_node_ref node);

	typedef std::map< node_id, std::list< sd_node_ref > > sd_dep_id_map;

	sd_dep_id_map construct_dep_id_map(sd_tree const& tree, sd_node_ref node, defn_dep_graph const& defn_dg);

	void print_dependency_graph(defn_dep_graph const& g, std::function< void(std::string) > sink);
	void print_dependency_graph(sd_dep_graph const& g, sd_tree const& tree, std::function< void(std::string) > sink);

}


#endif


