// schema_adjustment.cpp

#include "schema_validation.h"
#include "value_node.h"
#include "../sd_node_ref.h"


namespace ddl {

	// TODO: Seems wrong that removing unwanted children is done here, yet ensuring wanted children are in 
	// place is dealt with within the local instantiation visitor...
	struct adjust_visitor
	{
		typedef void result_type;

		adjust_visitor(sd_node_ref nref, sd_tree& tree):
			nref_(nref),
			tree_(tree)
		{}

		inline void operator() (boolean_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];

			// TODO: HACK for null schema node using boolean
			if(!nd.ptr())
			{
				node.data = value_node{};
//				cull_sd_children(nref_, tree_);
				return;
			}

			if(!node.data.is_bool() || !fits_schema(node.data, nd))
			{
				node.data = nd.default();
//				cull_sd_children(nref_, tree_);
			}
		}

		inline void operator() (integer_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];

			if(!node.data.is_int() || !fits_schema(node.data, nd))
			{
				node.data = nd.default();
//				cull_sd_children(nref_, tree_);
			}
		}

		inline void operator() (realnum_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];

			if(!node.data.is_real() || !fits_schema(node.data, nd))
			{
				node.data = nd.default();
//				cull_sd_children(nref_, tree_);
			}
		}

		inline void operator() (string_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];

			if(!node.data.is_string() || !fits_schema(node.data, nd))
			{
				node.data = nd.default();
//				cull_sd_children(nref_, tree_);
			}
		}

		inline void operator() (enum_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];

			if(!node.data.is_enum() || !fits_schema(node.data, nd))
			{
				node.data = nd.default();
//				cull_sd_children(nref_, tree_);
			}
		}

		inline void operator() (list_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];
			node.data = std::vector < value_node > {};

#if 0
/*			if(!val_.is_list() || !fits_schema(val_, nd))
			{
				val_ = std::vector < value_node > {};
			}
*/
			// Default is empty list
			std::vector< value_node > items;

			if(val_.is_list())
			{
				auto existing = val_.as_list();
				auto item_schemas = nd.entries();

				if(existing.size() != item_schemas.size())
				{
					// TODO: Suspect this should never happen?
					throw std::runtime_error("list schema item count mismatch");
				}

				// Iterate over existing items
				for(size_t i = 0; i < existing.size(); ++i)
				{
					auto item_val = existing[i];
					auto const& item_sch = item_schemas[i];

					// Recursively adjust value for item based on corresponding schema
					adjust_to_schema(item_val, item_sch);

					items.push_back(item_val);
				}
			}

			val_ = items;
#endif
		}

		inline void operator() (composite_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];
			node.data = std::map < node_name, value_node > {};

			/*
			// Remove child nodes which shouldn't exist
			std::list< sd_tree::node_descriptor > to_remove;
			for(auto c : tree_.children(nref_.nd))
			{
				auto const& edge = tree_[tree_.in_edge(c).first];
				if(edge.type != sd_edge_attribs::ChildType::Composite ||
					!nd.has_child(edge.child_name))
				{
					to_remove.push_back(c);
				}
			}
			for(auto c : to_remove)
			{
				tree_.remove_branch(c);
			}
			*/

#if 0
			std::map< node_name, value_node > components;
			// If already composite, start off with existing child values
			if(val_.is_composite())
			{
				components = val_.as_composite();

				// TODO: May not want to do this...?
				// Remove any children which do not exist in current schema
				for(auto it = std::begin(components); it != std::end(components); )
				{
					if(nd.has_child(it->first))
					{
						++it;
					}
					else
					{
						it = components.erase(it);
					}
				}
			}
			
			// Iterate over schema children
			auto sch_children = nd.children();
			for(auto const& c : sch_children)
			{
				auto id = c.first;
				auto sn = c.second;
				// If value for this child does not exist, default construct one
				if(components.find(id) == components.end())
				{
					components[id] = value_node{};
				}

				// Recursively adjust value for child based on child schema
				adjust_to_schema(components[id], sn);
			}

			val_ = components;
#endif
		}

		inline void operator() (conditional_sch_node const& nd) //const
		{
			auto& node = tree_[nref_.nd];
			node.data = value_node{};	// TODO: ?
		}

//		value_node& val_;
		sd_tree& tree_;
		sd_node_ref nref_;
	};

//	void adjust_to_schema(value_node& vals, sch_node const& sch)
	void adjust_to_schema(sd_node_ref nref, sd_tree& tree)
	{
		auto& node = tree[nref.nd];

		node.data.set_id(node.schema.get_id());

		auto vis = adjust_visitor{ nref, tree };
		node.schema.apply_visitor(vis);
	}

}



