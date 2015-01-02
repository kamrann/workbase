// sd_tree.cpp

#include "sd_tree.h"
#include "sd_node_ref.h"
#include "dependency_graph.h"
#include "ddl.h"
#include "values/schema_validation.h"

#ifdef _DEBUG
#define __DEBUG_OUTPUT 0
#endif


namespace ddl {

	sch_node instantiate_locally(sd_node_ref nref, sd_tree& tree);

	void update_sd_tree(sd_node_ref node, sd_tree& tree, bool include_root)
	{
		// Build graph of all dependents of 'node'
//		auto dep_graph = build_dependency_graph(tree, node);
		std::set< sd_node_ref, sd_node_ref_compare > processed;

		if(!include_root)
		{
			processed.insert(node);
		}

		auto defn_deps = build_defn_dependency_graph(tree[tree.get_root()].defn, tree[node.nd].defn);
		auto dep_order = defn_deps.topological_ordering();

		while(true)
		{
			// TODO: What about nodes that have a dependency via another node that has been removed, being
			// cut off from the set of nodes to be updated...

//			auto defn_deps = build_defn_dependency_graph(tree[tree.get_root()].defn, tree[node.nd].defn);
//			auto dep_order = defn_deps.topological_ordering();
#if __DEBUG_OUTPUT
			print_dependency_graph(defn_deps, [](std::string str) { std::cout << str << std::endl; });
#endif

			auto id_map = construct_dep_id_map(tree, node, defn_deps);

			auto next = sd_node_ref{};
			// Look for first node in the list that is not already processed
			for(auto const& rf : dep_order)
			{
				auto defn = defn_deps[rf].nd;
				auto id = defn.get_id();
				if(id_map.find(id) == id_map.end())
				{
					// No node with this id
					continue;
				}

				auto& matching = id_map.at(id);
				for(auto const& nref : matching)
				{
					if(processed.find(nref) == std::end(processed))
					{
						// Not yet processed, do this one next
						next = nref;
						break;
					}
				}

				if(next)
				{
					break;
				}
			}

			// TODO: maybe deal here with any null defn nodes?
			// ie. empty condition contents, should ideally be updated to ensure both schema and data
			// are set to null. currently since no id, they won't be deal with.
			// alternative could be to just set schema and data for them when setting the defn to null,
			// but this is kind of inconsistent with general approach...

			if(!next)
			{
				// No more to process, finished
				break;
			}

			auto& sd_node = tree[next.nd];
			// Reinstantiate the schema at this node only
			sd_node.schema = instantiate_locally(next, tree);
//			if(sd_node.schema)
			{
				// Adjust the data (if necessary) to conform to the reinstantiated schema
				adjust_to_schema(next, tree);
			}

			// Mark this node as done
			processed.insert(next);
		}
	}


	struct local_inst_visitor:
		public boost::static_visitor < sch_node >
	{
		local_inst_visitor(sd_tree& tree, sd_node_ref nref):
			tree_(tree),
			nref_(nref)
		{}

		result_type operator() (composite_defn_node const& comp_defn)
		{
			// composite
			/*
			for every child defn, ensure there is a corresponding child in the sd_tree, with its defn set to the
			defn from the composite's child list.
			resolve dependencies of these child defns and update the dependency graph to reflect this.
			*/
			
			// Assuming composite nodes are fixed and that therefore either all children exist, or none do (uninitialized)
			if(tree_.child_count(nref_.nd) == 0)
			{
				auto children = comp_defn.children();
				for(auto const& child : children)
				{
					auto name = child.first;
					auto cdefn = child.second;

					sd_node_attribs n_attribs;
// done below					n_attribs.defn = node_ref{ cdefn };
					sd_edge_attribs e_attribs;
					e_attribs.type = sd_edge_attribs::ChildType::Composite;
					e_attribs.child_name = name;
					tree_.add_node(nref_.nd, e_attribs, n_attribs);
				}

				// TODO: update dependencies - this is now done from scratch each time in update_sd_tree...
			}

			auto child_defns = comp_defn.children();
			for(auto c : tree_.children(nref_.nd))
			{
				auto cname = tree_[tree_.in_edge(c).first].child_name;
				auto cdefn = child_defns.at(cname);
				tree_[c].defn = cdefn;
			}

			sch_node sch = composite_sch_node{};
			sch.set_id(comp_defn.id_);
			return sch;
		}

		result_type operator() (conditional_defn_node const& cond_defn)
		{
			// conditional
			/*
			evaluate the condition against current sd_tree values. this gives rise to a defn_node.
			assign this defn to the conditional node's child in the sd_tree. resolve dependencies of this
			defn and update the dependency graph to reflect this.
			*/
			auto cdefn = cond_defn.evaluate(ref_resolver{ &resolve_reference }, navigator{ &tree_, nref_ });
			if(tree_.child_count(nref_.nd) == 0)
			{
				sd_node_attribs n_attribs;
				sd_edge_attribs e_attribs;
				e_attribs.type = sd_edge_attribs::ChildType::Conditional;
				tree_.add_node(nref_.nd, e_attribs, n_attribs);
			}
			auto c = tree_.get_leftmost_child(nref_.nd).first;
			tree_[c].defn = cdefn;

			// TODO: update dependencies - this is now done from scratch each time in update_sd_tree...

			sch_node sch = conditional_sch_node{};
			sch.set_id(cond_defn.id_);
			return sch;
		}

		result_type operator() (list_defn_node const& list_defn)
		{
			// TODO: min/max items enforced here??

			for(auto c : tree_.children(nref_.nd))
			{
				tree_[c].defn = list_defn.entrydefn();
			}

			sch_node sch = list_sch_node{};
			sch.set_id(list_defn.id_);
			return sch;
		}

		// leaf
		/*
		set the schema member of the sd_tree node from the defn.
		*/
		template < typename T >
		result_type operator() (T const& val)
		{
			return instantiate(val, navigator{ &tree_, nref_ }, ref_resolver{ &resolve_reference });
		}

		
		sd_tree& tree_;
		sd_node_ref nref_;
	};

	sch_node instantiate_locally(sd_node_ref nref, sd_tree& tree)
	{
		auto& node = tree[nref.nd];
		auto defn = node.defn;

		// Should we attempt to maintain data already existing at this node?
		// Only if all of the following conditions are satisfied...
		bool maintain =
			defn											// Non-null definition
			&& node.schema									// Non-null schema
			&& node.schema.get_id() == defn.get_id();		// Matching ids
		if(!maintain)
		{
			// Remove any existing children
			tree.clear_children(nref.nd);
		}
	
		if(defn)
		{
#if __DEBUG_OUTPUT
			std::cout << "instantiating: " << defn.get_name() << std::endl;
#endif

			local_inst_visitor vis{ tree, nref };
			return defn.apply_visitor(vis);
		}
		else
		{
			return{};
		}
	}

	void reset_node(sd_node_ref nref, sd_tree& tree)
	{
		// TODO:
		// This will cut off some indirect dependencies preventing them from being updated!!!!!!!!!!
		// Probably need to implement solution using defn dependencies to determine the ordering.
		tree[nref.nd].data = value_node{};
		tree.clear_children(nref.nd);
		update_sd_tree(nref, tree, true);
		
//		tree[nref.nd].schema = instantiate_locally(nref, tree);
//		update_sd_tree(nref, tree);
	}

}




