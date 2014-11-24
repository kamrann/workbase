// base_node_state.cpp

#include "base_node_state.h"
#include "node_type_choice.h"

#include "params/value_emitter.h"
#include "params/param_yaml.h"

#include "util/gtree/generic_tree_depthfirst.hpp"

#include <fstream>


namespace prm {
	namespace fsm {

		qualified_path base_node_state::nav_cmd_path(prm::nav_component const& nv)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;
			acc.set_match_method(param_accessor::Match::Anywhere, param_accessor::MatchComparison::PreferBeginningOrExact);

			if(nv.nav_index)
			{
				if(is_leaf_type(acc.param_here().type))
				{
					editor.output("cannot use index navigation at terminal node");
					return{};
				}

				unsigned int index = *nv.nav_index - 1;
				auto children = acc.children();
				if(index >= children.size())
				{
					editor.output("invalid nav index");
					return{};
				}

				auto it = std::next(std::begin(children), index);
				return *it;
			}
			else if(nv.dir)
			{
				switch(*nv.dir)
				{
					case nav_cmd::Direction::Up:
					if(nv.search_element)
					{
						auto path = acc.find_path_ancestor(nv.search_element->term);
						if(!path)
						{
							std::stringstream ss;
							ss << "no ancestor \'" << nv.search_element->term << "\'";
							editor.output(ss.str());
							return{};
						}
						return path;
					}
					else
					{
						auto path = acc.where();
						if(path.size() == 1)
						{
							editor.output("no parent");
							return{};
						}
						path.pop();
						return path;
					}
					break;

					case nav_cmd::Direction::Down:
					{
						auto path = acc.find_path_descendent(nv.search_element->term, nv.search_element->rpt_indices);
						if(!path)
						{
							std::stringstream ss;
							ss << "no descendent \'" << nv.search_element->term << "\', or ambiguous";
							editor.output(ss.str());
							return{};
						}
						return path;
					}
					break;

					default:
					throw std::runtime_error{ "invalid nav direction" };
				}
			}
			else
			{
				// Undirected nav
				auto path = acc.find_path(nv.search_element->term, nv.search_element->rpt_indices);
				if(!path)
				{
					std::stringstream ss;
					ss << "no node \'" << nv.search_element->term << "\', or ambiguous";
					editor.output(ss.str());
					return{};
				}
				return path;
			}
		}

		std::string base_node_state::format_value(param val)
		{
			//			typedef std::back_insert_iterator< std::string > sink_type;
			//			sink_type sink(dest);
			//			karma::generate(sink, grammar, value(s));

			std::stringstream ss;
			value_emitter< karma::ostream_iterator< char > > em;
			ss << karma::format(em, val);

			if(!ss)
			{
				ss.clear();
				ss << "!! output failure !!";
			}

			return ss.str();
		}

		std::string base_node_state::format_type_info(param_tree::param_data const& pd)
		{
			std::stringstream ss;
			ss << "[" << ParamTypeNameMap.left.at(pd.type) << "]";
			return ss.str();
		}

		std::string base_node_state::format_constraints(schema::schema_node sch)
		{
			std::stringstream ss;
			auto type = sch["type"].as< ParamType >();
			switch(type)
			{
				case ParamType::Enumeration:
				{
					ss << "in { ";
					for(auto node : sch["constraints"]["values"])
					{
						// TODO: shouldn't this be name rather than id? likewise for parsing/emitting
						ss << node["id"] << " ";
					}
					ss << "}";
				}
				break;
			}

			return ss.str();
		}


		sc::result base_node_state::react(clsm::ev_cmd< nav_cmd > const& cmd)
		{
			qualified_path new_path = nav_cmd_path(cmd);
			if(new_path)
			{
				context< paramtree_editor >().acc.move_to(new_path);
				return transit< node_type_choice >();// true;
			}
			else
			{
				return discard_event();// false;
			}
		}

		void base_node_state::on_list(clsm::ev_cmd< list_cmd > const& cmd)
		{
			auto const& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path;
			param_tree::param_data pd;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return;// todo: false;
				}
				pd = acc.find_param(path);
			}
			else
			{
				path = acc.where();
				pd = acc.param_here();
			}

			auto node = acc.node_at(path);

			unsigned int depth = 0;
			unsigned int max_depth = cmd.depth ? cmd.depth->value : 1u;
			unsigned int indent = 0;

			auto pre_op = [&](param_tree::tree_t::node_descriptor n)
			{
				if(depth <= max_depth)
				{
					auto& pt = editor.ptree;
					auto& tree = pt.tree();

					std::stringstream output;

					for(unsigned int i = 0; i < indent; ++i)
					{
						output << " ";
					}

					auto sibling_idx = tree.position_in_siblings(n);
					if(depth == 1)
					{
						// Index immediate children
						output << (sibling_idx + 1) << ". ";

						indent += 3;
					}

					output << tree[n].name;

					auto in = tree.in_edge(n).first;
					if(tree[in].repeat_idx)
					{
						output << " /" << (sibling_idx + 1);
					}

					output << ": ";
					if(is_leaf_type(tree[n].type))
					{
						output << format_value(*tree[n].value);
					}

					if(cmd.type_info)
					{
						output << " ";
						output << format_type_info(tree[n]);
						output << " ";
						auto path = pt.node_qpath(n);
						auto acc_copy = acc;
						acc_copy.move_to(path);
						output << format_constraints((*editor.provider)[path.unindexed()](acc_copy));
					}

					editor.output(output.str());
				}
				++depth;
				indent += 2;
			};

			auto post_op = [&](param_tree::tree_t::node_descriptor n)
			{
				--depth;
				indent -= 2;
				if(depth == 1)
				{
					indent -= 3;
				}
			};

			wb::gtree::depth_first_traversal(
				editor.ptree.tree(),
				node,
				pre_op,
				wb::gtree::null_op{},
				post_op
				);
		}

		void base_node_state::on_setvalue(clsm::ev_cmd< setvalue_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}
			else
			{
				path = acc.where();
			}

			auto& pd = acc.find_param(path);
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*editor.provider)[path.unindexed()](acc);

			bool ok = schema::validate_param(cmd.val, sch);
			if(!ok)
			{
				editor.output("invalid value");
				return;// false;
			}

			// TODO: shouldn't need to repeat lookup
			pd.value = cmd.val;

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("value set");
		}

		void base_node_state::on_save(clsm::ev_cmd< save_cmd > const& cmd)
		{
			auto const& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}
			else
			{
				path = acc.where();
			}

			//			fs::path filepath{ cmd.filename };
			std::ofstream file{ cmd.filename };
			if(!file.is_open())
			{
				editor.output("invalid filename");
				return;// false;
			}

			auto subtree_root = acc.node_at(path);
			auto as_yaml = editor.ptree.convert_to_yaml(subtree_root);

			file << YAML::Dump(as_yaml);

			editor.output("subtree saved");
		}

		void base_node_state::on_load(clsm::ev_cmd< load_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}
			else
			{
				path = acc.where();
			}

			auto as_yaml = YAML::LoadFile(cmd.filename);
			if(as_yaml.IsNull())
			{
				editor.output("invalid filename/file");
				return;// false;
			}

			auto subtree_root = acc.node_at(path);
			auto& tree = editor.ptree.tree();
			tree.clear_children(subtree_root);

			editor.ptree.generate_from_yaml(as_yaml, subtree_root);

			/* TODO: generate from yaml does not validate loaded params against schema...

			bool ok = schema::validate_param(cmd.val, sch);
			if(!ok)
			{
			os << "invalid value" << std::endl;
			return false;
			}
			*/
			// TODO: Overkill
			editor.reload_pt_schema();

			// TODO: if load into node which is above current location, current location may cease to exist,
			// in which case need to move the accessor, and transition to node_type_choice
			editor.output("subtree loaded");
		}

	}
}




