// yaml_conversion.cpp

#include "yaml_conversion.h"
#include "value_node.h"
#include "../sd_node_ref.h"
#include "../schema_defn/defn_node_holder.h"
#include "schema_validation.h"
#include "../path.h"
#include "navigator.h"
#include "../dependency_graph.h"

#ifdef _DEBUG
#define __DEBUG_OUTPUT 0
#endif


namespace ddl {

	struct yaml_export_visitor:
		public boost::static_visitor < YAML::Node >
	{
		yaml_export_visitor(sd_tree const& tree, sd_node_ref pos):
			tree_(tree),
			pos_(pos)
		{}

		result_type operator() (boolean_defn_node const& defn) const
		{
			YAML::Node y;
			y["boolean"] = tree_[pos_.nd].data.as_bool();
			return y;
		}

		result_type operator() (integer_defn_node const& defn) const
		{
			YAML::Node y;
			y["integer"] = tree_[pos_.nd].data.as_int();
			return y;
		}

		result_type operator() (realnum_defn_node const& defn) const
		{
			YAML::Node y;
			y["real"] = tree_[pos_.nd].data.as_real();
			return y;
		}
		
		result_type operator() (string_defn_node const& defn) const
		{
			YAML::Node y;
			y["string"] = tree_[pos_.nd].data.as_string();
			return y;
		}

		result_type operator() (enum_defn_node const& defn) const
		{
			YAML::Node y;
			y["enum"] = tree_[pos_.nd].data.as_enum_str();
			return y;
		}

		result_type operator() (list_defn_node const& defn) const
		{
			YAML::Node y;
			YAML::Node lst = YAML::Load("[]");
			for(auto const& item : tree_.children(pos_.nd))
			{
				lst.push_back(export_yaml(tree_, item));
			}
			y["list"] = lst;
			return y;
		}
	
		result_type operator() (composite_defn_node const& defn) const
		{
			YAML::Node y;
			YAML::Node mp;
			for(auto const& entry : tree_.children(pos_.nd))
			{
				auto name = tree_[tree_.in_edge(entry).first].child_name;
				mp[name] = export_yaml(tree_, entry);
			}
			y["composite"] = mp;
			return y;
		}

		result_type operator() (conditional_defn_node const& defn) const
		{
			YAML::Node y;
			y["conditional"] = export_yaml(tree_, tree_.get_leftmost_child(pos_.nd).first);
			return y;
		}

		sd_tree const& tree_;
		sd_node_ref pos_;
	};

	YAML::Node export_yaml(sd_tree const& tree, sd_node_ref pos)
	{
		auto const& node = tree[pos.nd];
		if(node.defn)
		{
			yaml_export_visitor vis{ tree, pos };
			return node.defn.apply_visitor(vis);
		}
		else
		{
			// Null condition child
			return YAML::Node{};
		}
	}

	void build_value_map(YAML::Node yaml, path pth, std::map< path, value_node >& valmap)
	{
		auto it = yaml.begin();
		// TODO: YAML convert template
		auto key = it->first.as< std::string >();
		auto yaml_val = it->second;
		if(key == "boolean")
		{
			valmap[pth] = yaml_val.as< boolean_sch_node::value_t >();
		}
		else if(key == "integer")
		{
			valmap[pth] = yaml_val.as< integer_sch_node::value_t >();
		}
		else if(key == "real")
		{
			valmap[pth] = yaml_val.as< realnum_sch_node::value_t >();
		}
		else if(key == "string")
		{
			valmap[pth] = yaml_val.as< string_sch_node::value_t >();
		}
		else if(key == "enum")
		{
			valmap[pth] = yaml_val.as< enum_sch_node::str_value_t >();
		}
		else if(key == "list")
		{
			valmap[pth] = std::vector < value_node > { yaml_val.size(), value_node{} };

			size_t idx = 0;
			for(auto item : yaml_val)
			{
				auto child_pth = pth;
				child_pth.append(idx);
				build_value_map(item, child_pth, valmap);

				++idx;
			}
		}
		// TODO: Maybe add composite and conditional to value map for completeness?
		else if(key == "composite")
		{
			for(auto item : yaml_val)
			{
				auto name = item.first.as< std::string >();
				auto child_pth = pth;
				child_pth.append(name);
				build_value_map(item.second, child_pth, valmap);
			}
		}
		else if(key == "conditional")
		{
			if(!yaml_val.IsNull()
				// TODO: This check is due to a yaml-cpp bug.
				// See: https://code.google.com/p/yaml-cpp/issues/detail?id=230&colspec=ID%20Type%20Status%20Priority%20Milestone%20Component%20Owner%20Summary
				&& !(yaml_val.IsScalar() && yaml_val.Scalar() == "~")
				)
			{
				auto child_pth = pth;
				child_pth.append(path::conditional{});
				build_value_map(yaml_val, child_pth, valmap);
			}
		}
		else
		{
			throw std::runtime_error{ "invalid ddl yaml data file" };
		}
	}

	struct assign_import_value_visitor:
		public boost::static_visitor < bool >
	{
		assign_import_value_visitor(sd_tree& tree, sd_node_ref pos, value_node val):
			tree_(tree),
			pos_(pos),
			val_(val)
		{}

		result_type operator() (composite_defn_node const& defn)
		{
			return true;
		}

		result_type operator() (list_defn_node const& defn)
		{
			if(!val_.is_list())
			{
				return false;
			}

			for(size_t i = 0; i < val_.as_list().size(); ++i)
			{
				auto edge = sd_edge_attribs{};
				edge.type = sd_edge_attribs::ChildType::ListItem;
				edge.item_idx = i;
				auto node = sd_node_attribs{};
				node.defn = defn.entrydefn();
				tree_.add_node(pos_.nd, edge, node);
			}

			return true;
		}

		result_type operator() (conditional_defn_node const& defn)
		{
			return true;
		}

		result_type operator() (boolean_defn_node const& defn)
		{
			return val_.is_bool() && assign_terminal();
		}

		result_type operator() (integer_defn_node const& defn)
		{
			return val_.is_int() && assign_terminal();
		}

		result_type operator() (realnum_defn_node const& defn)
		{
			return val_.is_real() && assign_terminal();
		}

		result_type operator() (string_defn_node const& defn)
		{
			return val_.is_string() && assign_terminal();
		}

		result_type operator() (enum_defn_node const& defn)
		{
			if(val_.is_enum() && assign_terminal())
			{
				// TODO: Feel like the data component should not be stored as part of the value node, only the id string
				// id string -> data association is contained within the schema node.
				// seems reasonable to not duplicate this, and to have to refer back to the schema node when
				// we need the associated data value - ie. in an sd_tree/navigator::get_node_value_as<>() method
				auto sch = tree_[pos_.nd].schema.get_as< enum_sch_node >();
				auto vals = tree_[pos_.nd].data.as_enum();
				for(auto& elem : vals)
				{
					elem.data = sch.get_data_from_id_string(elem.str);
				}
				tree_[pos_.nd].data = vals;
				return true;
			}
			else
			{
				return false;
			}
		}


		bool assign_terminal()
		{
			auto const& sch = tree_[pos_.nd].schema;
			bool ok = fits_schema(val_, sch);
			if(!ok)
			{
				return false;
			}

			tree_[pos_.nd].data = val_;
			return true;
		}

		sd_tree& tree_;
		sd_node_ref pos_;
		value_node val_;
	};

	bool import_yaml(YAML::Node yaml, sd_tree& tree, sd_node_ref pos)
	{
		if(!yaml.IsMap() || yaml.size() != 1)
		{
			throw std::runtime_error{ "invalid ddl yaml data file" };
		}

		// Can't load recursively since need to respect dependency order.
		// Approach is to reset the root node at which we are loading, then build a list of
		// path/value pairs for every terminal node value specified in the yaml.
		// Then attempt to assign these values to their respective nodes in the dependency order,
		// updating the tree after every successful assignment.

		reset_node(pos, tree);

		std::map< path, value_node > valmap;
		navigator nav{ &tree, pos.nd };
		build_value_map(yaml, nav.where(), valmap);

		auto defn_deps = build_defn_dependency_graph(tree[tree.get_root()].defn, tree[pos.nd].defn);
		auto dep_order = defn_deps.topological_ordering();

		while(!valmap.empty())
		{
			// Generate ordering from underlying definition dependency graph
//			auto defn_deps = build_defn_dependency_graph(tree[tree.get_root()].defn, tree[pos.nd].defn);
//			auto dep_order = defn_deps.topological_ordering();
				//sd_deps.topological_ordering();

			// Candidate nodes for next load assignment are those existing nodes which are dependencies of the node
			// being loaded.
			// Construct a map so we can access these candidates by node id
			auto id_map = construct_dep_id_map(tree, pos, defn_deps);

#if __DEBUG_OUTPUT
			std::cout << "Import dependents:" << std::endl;
			for(auto vtx : dep_order)
			{
				auto defn = defn_deps[vtx].nd;
				std::cout << defn.get_id() << ": " << defn.get_name() << std::endl;
			}
#endif

			// Now iterate in defn dependency order, until we find a node which has not yet been assigned
			auto it = valmap.end();
			for(auto vtx : dep_order)
			{
				auto defn = defn_deps[vtx].nd;
				auto id = defn.get_id();
				if(id_map.find(id) == id_map.end())
				{
					// No node with this id
					continue;
				}

				auto& matching = id_map.at(id);
				for(auto const& nref : matching)
				{
					/*				auto nref = sd_deps[vtx];
					*/
					nav = navigator{ &tree, nref };
					auto dep_path = nav.where();
					it = valmap.find(dep_path);
					if(it != valmap.end())
					{
						// This is the next terminal node which should be assigned
						break;
					}
				}

				if(it != valmap.end())
				{
					break;
				}
			}

			if(it == valmap.end())
			{
				// Something went wrong...
				return false;
			}

#ifdef _DEBUG
			std::cout << "Loading value: " << it->first.to_string() << std::endl;
#endif

			nav = navigator{ &tree, tree.get_root() };
			nav = nav[it->first];
			auto cur_loc = nav.get_ref();
			auto& cur_node = tree[cur_loc.nd];

			assign_import_value_visitor vis{ tree, cur_loc, it->second };
			if(!cur_node.defn.apply_visitor(vis))
			{
				return false;
			}

			//auto pos = editor.nav.where();
			update_sd_tree(/*sd_node_ref{ tree.get_root() }*/cur_loc, tree, false);
			//editor.nav = editor.nav[pos];

			valmap.erase(it);
		}

		return true;
	}

}




