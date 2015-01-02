// dependency_graph.cpp

#include "dependency_graph.h"
#include "ddl.h"

#include <boost/variant/static_visitor.hpp>
#include <boost/graph/topological_sort.hpp>


#include <map>


namespace ddl {

	typedef dependency_graph::vertex_descriptor vertex_t;
	typedef dependency_graph::vertex_descriptor edge_t;

	typedef std::map< node_ref, vertex_t, node_ref_compare > build_node_map_t;

	struct node_deps_visitor:
		public boost::static_visitor < dep_list >
	{
		result_type operator() (enum_defn_node const& nd) const
		{
			return nd.deps();
		}

		result_type operator() (conditional_defn_node const& nd) const
		{
			return nd.deps();
		}

		template < typename T >
		result_type operator() (T const& nd) const
		{
			return{};
		}
	};

	inline dep_list get_node_deps(defn_node dn)
	{
		return dn.apply_visitor(node_deps_visitor{});
	}


	void recursive_build(dependency_graph& g, defn_node dn, build_node_map_t& nd_map);

	struct build_visitor:
		public boost::static_visitor < void >
	{
		build_visitor(dependency_graph& g, build_node_map_t& nd_map):
			g_(g),
			nd_map_(nd_map)
		{

		}

		result_type operator() (list_defn_node const& list)
		{
			auto entry_dn = list.entrydefn();
			recursive_build(g_, entry_dn, nd_map_);
		}

		result_type operator() (composite_defn_node const& composite)
		{
			auto children = composite.children();
			for(auto const& c_dn : children)
			{
				recursive_build(g_, c_dn.second, nd_map_);
			}
		}

		result_type operator() (conditional_defn_node const& conditional)
		{
			for(auto const& comp : conditional.components_)
			{
				recursive_build(g_, comp.nd, nd_map_);
			}
		}

		template < typename Terminal >
		result_type operator() (Terminal const& term)
		{

		}

		dependency_graph& g_;
		build_node_map_t& nd_map_;
	};

	void recursive_build(dependency_graph& g, defn_node dn, build_node_map_t& nd_map)
	{
		// Add node dn to graph and node map, assuming it has not been added already
		if(nd_map.find(node_ref{ dn }) == std::end(nd_map))
		{
			auto dn_vtx = boost::add_vertex(dg_vertex_props{ dn }, g);
			nd_map[node_ref{ dn }] = dn_vtx;
		}

		// Iterate over direct dependencies of dn
		auto deps = get_node_deps(dn);
		for(auto const& d : deps)
		{
			auto props = dg_edge_props{ DependencyType::Generic };
			auto src = nd_map.at(node_ref{ dn });
			// If the dependency is not already in the graph/node map, then add it
			if(nd_map.find(d) == std::end(nd_map))
			{
				auto d_vtx = boost::add_vertex(dg_vertex_props{ d }, g);
				nd_map[d] = d_vtx;
			}
			auto dst = nd_map.at(d);
			// Add a directed edge from dn to the dependency
			boost::add_edge(src, dst, props, g);
		}

		// Allow non-terminal nodes to continue building recursively
		build_visitor vis{ g, nd_map };
		dn.apply_visitor(vis);
	}

	dependency_graph build_defn_dependency_graph(defn_node dn)
	{
		dependency_graph g;
		if(!dn)
		{
			return g;
		}

		build_node_map_t nd_map;
		recursive_build(g, dn, nd_map);
		return g;
	}

	void print_dependency_graph(dependency_graph const& g, std::function< void(std::string) > sink)
	{
		auto ordering = g.topological_ordering();

		for(auto vtx : ordering)
		{
			std::stringstream ss;

			auto const& props = g[vtx];
			ss << "Node [" << props.nd.get_name() << "] depends on: ";

			for(auto out = boost::out_edges(vtx, g); out.first != out.second; ++out.first)
			{
				auto edge = *out.first;
				auto props = g[edge];
				// TODO: if(props.dep_type...

				auto dep_vtx = boost::target(edge, g);
				auto dep = g[dep_vtx];
				ss << dep.nd.get_name() << ", ";
			}

			sink(ss.str());
		}
	}

	std::vector< vertex_t > dependency_graph::topological_ordering() const
	{
		std::vector< vertex_t > output;
		boost::topological_sort(*this, std::back_inserter(output));
		return output;
	}

}



