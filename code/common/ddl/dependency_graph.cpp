// dependency_graph.cpp

#include "dependency_graph.h"
#include "ddl.h"

#include <boost/variant/static_visitor.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>

#include <map>

#ifdef _DEBUG
#define __DEBUG_OUTPUT 0
#endif


namespace ddl {

	template < typename Graph >
	class is_connected_visitor:
		public boost::default_dfs_visitor
	{
	public:
		typedef Graph graph_t;
		typedef typename graph_t::vertex_descriptor vertex_t;
		typedef typename graph_t::edge_descriptor edge_t;
		typedef typename graph_t::vertex_property_type vertex_props_t;

	public:
		is_connected_visitor(std::function< bool(vertex_props_t const&) > compare, bool& is_connected):
			compare_(compare),
			is_connected_(is_connected)
		{
			is_connected_ = false;
		}

		void discover_vertex(vertex_t u, graph_t const& g)
		{
			if(//g[u].nd == node_)
				compare_(g[u]))
			{
				is_connected_ = true;
			}
		}

//		vertex_t node_;
		std::function< bool(vertex_props_t const&) > compare_;
		bool& is_connected_;
	};

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
		return dn ? dn.apply_visitor(node_deps_visitor{}) : dep_list{};
	}


	typedef defn_dep_graph::vertex_descriptor defn_vertex_t;
	typedef defn_dep_graph::edge_descriptor defn_edge_t;

	typedef std::map< node_ref, defn_vertex_t, node_ref_compare > defn_build_node_map_t;

	void add_node_dependencies(
		defn_dep_graph& g,
		node_ref nref,
		node_ref parent,
		defn_build_node_map_t& nd_map)
	{
		// Add node nref to graph and node map, assuming it has not been added already
		if(nd_map.find(nref) == std::end(nd_map))
		{
			auto vtx = boost::add_vertex(defn_dg_vertex_props(nref), g);
			nd_map[nref] = vtx;
		}

		auto src = nd_map.at(nref);

		// TODO: Wanted?
		// First add the node's parent as a dependency
		if(parent.nd)
		{
			// If the parent is not already in the graph/node map, then add it
			if(nd_map.find(parent) == std::end(nd_map))
			{
				auto p_vtx = boost::add_vertex(defn_dg_vertex_props(parent), g);
				nd_map[parent] = p_vtx;
			}

			auto parent_vtx = nd_map.at(parent);
			auto props = dg_edge_props{ DependencyType::Parent };
			boost::add_edge(src, parent_vtx, props, g);
		}

		// Iterate over direct dependencies of nref
		auto deps = get_node_deps(nref.nd);
		for(auto const& d : deps)
		{
			auto props = dg_edge_props{ DependencyType::Generic };
			// If the dependency is not already in the graph/node map, then add it
			if(nd_map.find(d) == std::end(nd_map))
			{
				auto d_vtx = boost::add_vertex(defn_dg_vertex_props(d), g);
				nd_map[d] = d_vtx;
			}
			auto dst = nd_map.at(d);
			// Add a directed edge from nref to the dependency
			boost::add_edge(src, dst, props, g);
		}
	}

	void recursive_build(defn_dep_graph& g, defn_node dn, defn_node parent, defn_build_node_map_t& nd_map);

	struct build_visitor:
		public boost::static_visitor < void >
	{
		build_visitor(defn_dep_graph& g, defn_build_node_map_t& nd_map):
			g_(g),
			nd_map_(nd_map)
		{

		}

		result_type operator() (list_defn_node const& list)
		{
			auto entry_dn = list.entrydefn();
			recursive_build(g_, entry_dn, list, nd_map_);
		}

		result_type operator() (composite_defn_node const& composite)
		{
			auto children = composite.children();
			for(auto const& c_dn : children)
			{
				recursive_build(g_, c_dn.second, composite, nd_map_);
			}
		}

		result_type operator() (conditional_defn_node const& conditional)
		{
			for(auto const& comp : conditional.components_)
			{
				recursive_build(g_, comp.nd, conditional, nd_map_);
			}
			if(conditional.default_)
			{
				recursive_build(g_, conditional.default_, conditional, nd_map_);
			}
		}

		template < typename Terminal >
		result_type operator() (Terminal const& term)
		{

		}

		defn_dep_graph& g_;
		defn_build_node_map_t& nd_map_;
	};

	void recursive_build(defn_dep_graph& g, defn_node dn, defn_node parent, defn_build_node_map_t& nd_map)
	{
		add_node_dependencies(g, node_ref{ dn }, parent, nd_map);

		// Allow non-terminal nodes to continue building recursively
		build_visitor vis{ g, nd_map };
		dn.apply_visitor(vis);
	}

	/*
	Reimplement this using reverse_graph() to flip the edges so they go from dependency to dependent,
	then use a linear walk over vertices to find the starting vertex, and from there somehow* just take the 
	subgraph formed by following edges from that point. Finally reapply reverse_graph() on the result.

	*Simple way would be to use depth_first_visit to record all vertices in that subgraph.
	Then linear walk the graph again, removing all vertices that are not in the stored set.

	Or perhaps easier to just build a new graph up from scratch during the depth_first_visit?
	*/

	template < typename SrcGraph, typename OutGraph = SrcGraph >
	class copy_sub_dag_visitor:
		public boost::default_dfs_visitor
	{
	public:
		typedef SrcGraph src_graph_t;
		typedef OutGraph out_graph_t;
		typedef typename src_graph_t::vertex_descriptor src_vertex_t;
		typedef typename src_graph_t::edge_descriptor src_edge_t;
		typedef typename out_graph_t::vertex_descriptor out_vertex_t;
		typedef typename out_graph_t::edge_descriptor out_edge_t;
		typedef typename boost::property_map< src_graph_t, boost::vertex_bundle_t >::type src_vertex_props_t;
		typedef typename boost::property_map< src_graph_t, boost::edge_bundle_t >::type src_edge_props_t;
		typedef typename boost::property_map< out_graph_t, boost::vertex_bundle_t >::type out_vertex_props_t;
		typedef typename boost::property_map< out_graph_t, boost::edge_bundle_t >::type out_edge_props_t;

	public:
		copy_sub_dag_visitor(out_graph_t& result):
			result_(result)
		{

		}

		void discover_vertex(src_vertex_t u, src_graph_t const& g)
		{
			if(vtx_mp_.find(u) == vtx_mp_.end())
			{
				vtx_mp_[u] = boost::add_vertex(g[u], result_);
			}
		}

		void examine_edge(src_edge_t e, src_graph_t const& g)
		{
			auto src = boost::source(e, g);
			auto dst = boost::target(e, g);
			if(vtx_mp_.find(dst) == vtx_mp_.end())
			{
				vtx_mp_[dst] = boost::add_vertex(g[dst], result_);
			}

			boost::add_edge(vtx_mp_.at(src), vtx_mp_.at(dst), g[e], result_);
		}

		out_graph_t& result_;
		std::map< src_vertex_t, out_vertex_t > vtx_mp_;
	};

	template < typename Graph >
	class connected_visitor:
		public boost::default_dfs_visitor
	{
	public:
		typedef Graph graph_t;
		typedef typename graph_t::vertex_descriptor vertex_t;
		typedef typename graph_t::edge_descriptor edge_t;
		typedef typename boost::property_map< graph_t, boost::vertex_bundle_t >::type vertex_props_t;
		typedef typename boost::property_map< graph_t, boost::edge_bundle_t >::type edge_props_t;

	public:
		connected_visitor(std::set< vertex_t >& result):
			result_(result)
		{

		}

		void discover_vertex(vertex_t u, graph_t const& g)
		{
			result_.insert(u);
		}

		std::set< vertex_t >& result_;
	};

	// This removes all vertices that do not have a dependency (direct or indirect) on nref
	void dependent_upon(defn_dep_graph& g, defn_node dn)
	{
		typedef boost::reverse_graph< defn_dep_graph, defn_dep_graph& > rev_g_t;
		rev_g_t rev{ g };

		auto root_vtx = rev_g_t::null_vertex();
		for(auto rg = boost::vertices(rev); rg.first != rg.second; ++rg.first)
		{
			auto vtx = *rg.first;
			if(rev[vtx].nd.get_id() == dn.get_id())
			{
				root_vtx = vtx;
			}
		}

		if(root_vtx == rev_g_t::null_vertex())
		{
			throw std::runtime_error("dependency root not found in graph");
		}

		std::set< rev_g_t::vertex_descriptor > conn_nodes;
		connected_visitor< rev_g_t > conn_vis{ conn_nodes };
		typedef std::map < rev_g_t::vertex_descriptor, boost::default_color_type > color_map_t;
		color_map_t color_map_cont;
		boost::associative_property_map < color_map_t > color_map{ color_map_cont };
		boost::depth_first_visit(rev, root_vtx, conn_vis, color_map);

		// TODO: Don't know if okay to use descriptors from the reverse graph on the original,
		// but trying to call clear_vertex on the reverse adapter doesn't seem to work...
		rev_g_t::vertex_iterator v_i, v_end;
		std::tie(v_i, v_end) = boost::vertices(g);
		for(auto v_next = v_i; v_i != v_end; v_i = v_next)
		{
			++v_next;
			auto vtx = *v_i;
			if(conn_nodes.find(vtx) == conn_nodes.end())
			{
				boost::clear_vertex(vtx, g);
				boost::remove_vertex(vtx, g);
			}
		}

#if 0
		typedef boost::reverse_graph< defn_dep_graph > rev_g_t;
		rev_g_t rev{ g };
		auto root_vtx = rev_g_t::null_vertex();
		for(auto rg = boost::vertices(rev); rg.first != rg.second; ++rg.first)
		{
			auto vtx = *rg.first;
			auto const& crev = rev;	// weird issue
			if(crev[vtx].nd.get_id() == dn.get_id())
			{
				root_vtx = vtx;
			}
		}

		if(root_vtx == rev_g_t::null_vertex())
		{
			throw std::runtime_error("dependency root not found in graph");
		}

		auto result = defn_dep_graph{};

		copy_sub_dag_visitor< rev_g_t, defn_dep_graph > copy_vis{ result };

/*		typedef boost::reverse_graph< defn_dep_graph, defn_dep_graph& > mut_rev_g_t;
		auto rev_result = mut_rev_g_t{ result };
		copy_sub_dag_visitor< rev_g_t, mut_rev_g_t > copy_vis{ rev_result };
*/
		typedef std::map < defn_vertex_t, boost::default_color_type > color_map_t;
		color_map_t color_map_cont;
		boost::associative_property_map < color_map_t > color_map{ color_map_cont };
		boost::depth_first_visit(rev, root_vtx, copy_vis, color_map);

		g = defn_dep_graph{};
		hassle with named params...
		boost::transpose_graph(result, g);
			//result;
#endif
	}

#if 0
	// This removes all vertices that do not have a dependency (direct or indirect) on nref
	void dependent_upon(defn_dep_graph& g, defn_node dn)
	{
		std::list< defn_vertex_t > to_remove;
		for(auto rg = boost::vertices(g); rg.first != rg.second; ++rg.first)
		{
			auto vtx = *rg.first;
			// Starting at vtx, can we get to nref?
			bool is_connected = false;
			is_connected_visitor< defn_dep_graph > vis{
				[dn](defn_dg_vertex_props const& props)
				{
					return props.nd.get_id() == dn.get_id();
				},
				//nref,
				is_connected
			};
			typedef std::map < defn_vertex_t, boost::default_color_type > color_map_t;
			color_map_t color_map_cont;
			boost::associative_property_map < color_map_t > color_map{ color_map_cont };
			boost::depth_first_visit(g, vtx, vis, color_map);

			if(!is_connected)
			{
				to_remove.push_back(vtx);
			}
		}

		for(auto vtx : to_remove)
		{
			boost::clear_vertex(vtx, g);
			boost::remove_vertex(vtx, g);
		}
	}
#endif
	defn_dep_graph build_defn_dependency_graph(defn_node hierarchy_root, defn_node dep_root)
	{
		defn_dep_graph g;
		if(!hierarchy_root || !dep_root)
		{
			return g;
		}

		// Build up the complete dependency graph
		defn_build_node_map_t nd_map;
		recursive_build(g, hierarchy_root, defn_node{}, nd_map);

		// And then get the subgraph of nodes dependent on nref
		dependent_upon(g, dep_root);
		return g;
	}

#if 0
	void recursive_build(defn_dep_graph& g, defn_node dn, defn_build_node_map_t& nd_map)
	{
		// Add node dn to graph and node map, assuming it has not been added already
		if(nd_map.find(node_ref{ dn }) == std::end(nd_map))
		{
			auto dn_vtx = boost::add_vertex(defn_dg_vertex_props{ dn }, g);
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
				auto d_vtx = boost::add_vertex(defn_dg_vertex_props{ d }, g);
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

	// TODO: Currently this includes child->parent dependencies.
	defn_dep_graph build_defn_dependency_graph(defn_node dn)
	{
		defn_dep_graph g;
		if(!dn)
		{
			return g;
		}

		defn_build_node_map_t nd_map;
		recursive_build(g, dn, nd_map);
		return g;
	}
#endif



	typedef sd_dep_graph::vertex_descriptor sd_vertex_t;
	typedef sd_dep_graph::edge_descriptor sd_edge_t;

	typedef std::map< sd_node_ref, sd_vertex_t, sd_node_ref_compare > sd_build_node_map_t;

	void add_node_dependencies(
		sd_dep_graph& g,
		sd_node_ref nref,
		sd_tree const& tree,
		sd_build_node_map_t& nd_map)
	{
		auto const& node = tree[nref.nd];

		// Add node nref to graph and node map, assuming it has not been added already
		if(nd_map.find(nref) == std::end(nd_map))
		{
			auto vtx = boost::add_vertex(sd_dg_vertex_props(nref), g);
			nd_map[nref] = vtx;
		}

		auto src = nd_map.at(nref);

		// TODO: Wanted?
		// First add the node's parent as a dependency
		auto parent = tree.get_parent(nref.nd);
		if(parent.second)
		{
			auto parent_ref = sd_node_ref{ parent.first };
			// If the parent is not already in the graph/node map, then add it
			if(nd_map.find(parent_ref) == std::end(nd_map))
			{
				auto p_vtx = boost::add_vertex(sd_dg_vertex_props(parent_ref), g);
				nd_map[parent_ref] = p_vtx;
			}

			auto parent_vtx = nd_map.at(parent_ref);
			auto props = dg_edge_props{ DependencyType::Parent };
			boost::add_edge(src, parent_vtx, props, g);
		}

		// Iterate over direct dependencies of nref
		auto defn_deps = get_node_deps(node.defn);
		for(auto const& dd : defn_deps)
		{
			// Resolve the dependency reference (possibly to 0, 1 or multiple instantiated nodes)
			auto sd_deps = resolve_reference(dd, navigator{ &tree, nref });

			for(auto const& sdd : sd_deps)
			{
				auto props = dg_edge_props{ DependencyType::Generic };
				// If the dependency is not already in the graph/node map, then add it
				if(nd_map.find(sdd) == std::end(nd_map))
				{
					auto d_vtx = boost::add_vertex(sd_dg_vertex_props(sdd), g);
					nd_map[sdd] = d_vtx;
				}
				auto dst = nd_map.at(sdd);
				// Add a directed edge from nref to the dependency
				boost::add_edge(src, dst, props, g);
			}
		}
	}

	// TODO: If going to use this, implement as above for defn graph (much more efficient)
	// This removes all vertices that do not have a dependency (direct or indirect) on nref
	void dependent_upon(sd_dep_graph& g, sd_node_ref nref)
	{
		assert(false); // see above

		std::list< sd_vertex_t > to_remove;
		for(auto rg = boost::vertices(g); rg.first != rg.second; ++rg.first)
		{
			auto vtx = *rg.first;
			// Starting at vtx, can we get to nref?
			bool is_connected = false;
			is_connected_visitor< sd_dep_graph > vis{
				[nref](sd_dg_vertex_props const& props)
				{
					return props.nd == nref.nd;
				},
				//nref.nd,
				is_connected };
			typedef std::map < sd_vertex_t, boost::default_color_type > color_map_t;
			color_map_t color_map_cont;
			boost::associative_property_map < color_map_t > color_map{ color_map_cont };
			boost::depth_first_visit(g, vtx, vis, color_map);

			if(!is_connected)
			{
				to_remove.push_back(vtx);
			}
		}

		for(auto vtx : to_remove)
		{
			boost::clear_vertex(vtx, g);
			boost::remove_vertex(vtx, g);
		}
	}

	sd_dep_graph build_dependency_graph(sd_tree const& tree, sd_node_ref nref)
	{
		sd_dep_graph g;
		if(!nref)
		{
			return g;
		}

		// Build up the complete dependency graph
		sd_build_node_map_t nd_map;
		for(auto node : tree.nodes())
		{
			add_node_dependencies(g, sd_node_ref{ node }, tree, nd_map);
		}

		// And then get the subgraph of nodes dependent on nref
		dependent_upon(g, nref);
		return g;
	}

	sd_dep_id_map construct_dep_id_map(sd_tree const& tree, sd_node_ref node, defn_dep_graph const& defn_dg)
	{
//		auto sd_deps = build_dependency_graph(tree, node.nd);
		
		/* TODO:
		for now, forgetting the sd dependency graph and building a list of dependent candidates from all
		nodes in the sd tree (TODO: with an id matching a node in the defn dependency graph).

		this is so we don't need to worry about resolving awkward references needing a different source
		lookup context. eventually, references may have to be hierarchical (currently a dep_list of 
		dependencies just gets flattened to a list of node references), so that any sequence of lookup
		contexts is retained.

		also, ideally we want to maintain an sd dependency graph which deals with dependent nodes which may be
		cut off by intermediate dependencies being deleted from the tree during update, and which also 
		saves work by checking when a node is reinstantiated, whether the value was actually altered, and
		if not, removes incoming dependent edges from the graph. this way, nodes (often entire subgraphs)
		can be discarded without being updated when they become disjoint from the dep graph root node.
		*/

#if __DEBUG_OUTPUT
		std::cout << "S-D dependents:" << std::endl;
#endif

		std::set< node_id > dep_ids;
		for(auto rg = boost::vertices(defn_dg); rg.first != rg.second; ++rg.first)
		{
			auto id = defn_dg[*rg.first].nd.get_id();
			dep_ids.insert(id);
		}

		sd_dep_id_map id_map;
//		for(auto rg = boost::vertices(sd_deps); rg.first != rg.second; ++rg.first)
		for(auto vtx : tree.nodes())
		{
			//auto nref = sd_deps[*rg.first];
			auto nref = sd_node_ref{ vtx };

			auto defn = tree[vtx].defn;
			if(defn)
			{
				auto id = defn.get_id();

				if(dep_ids.find(id) != dep_ids.end())
				{
					id_map[id].push_back(nref);
				}
			}

#if __DEBUG_OUTPUT
			if(defn)
			{
				std::cout << defn.get_id() << ": " << defn.get_name() << std::endl;
			}
			else
			{
				std::cout << "null defn" << std::endl;
			}
#endif
		}

		return id_map;
	}


	void print_dependency_graph(defn_dep_graph const& g, std::function< void(std::string) > sink)
	{
		auto ordering = g.topological_ordering();

		for(auto vtx : ordering)
		{
			std::stringstream ss;

			auto const& props = g[vtx];
			ss << "Defn [" << props.nd.get_name() << "] depends on: ";

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
	
	void print_dependency_graph(sd_dep_graph const& g, sd_tree const& tree, std::function< void(std::string) > sink)
	{
		auto ordering = g.topological_ordering();

		for(auto vtx : ordering)
		{
			std::stringstream ss;

			auto const& props = g[vtx];
			auto const& defn = tree[props.nd].defn;
			auto name = defn ? defn.get_name() : std::string{ "<null>" };
			ss << "Node [" << name << "] depends on: ";

			for(auto out = boost::out_edges(vtx, g); out.first != out.second; ++out.first)
			{
				auto edge = *out.first;
				auto props = g[edge];
				// TODO: if(props.dep_type...

				auto dep_vtx = boost::target(edge, g);
				auto dep = g[dep_vtx];
				ss << tree[dep.nd].defn.get_name() << ", ";
			}

			sink(ss.str());
		}
	}

}



