// base_node_state.cpp

#include "base_node_state.h"
#include "node_type_choice.h"

#include "../terminal_io/value_emitter.h"
#include "../values/yaml_conversion.h"
#include "../values/schema_validation.h"

#include "util/gtree/generic_tree_depthfirst.hpp"

#include <fstream>


namespace ddl {
	namespace fsm {

		base_node_state::base_node_state(my_context ctx): base(ctx)
		{
			auto const is_terminal = at_leaf_node();
			auto const enable_indexed = !is_terminal;
			auto const require_nav = !is_terminal;

			reg_cmd< nav_cmd >(wrap_grammar< nav_cmd_parser< clsm::iter_t > >(enable_indexed));
			reg_cmd< list_cmd >(wrap_grammar< list_cmd_parser< clsm::iter_t > >(enable_indexed));
			reg_cmd< setvalue_cmd >(wrap_grammar< setvalue_cmd_parser< clsm::iter_t > >(enable_indexed, require_nav));
			reg_cmd< reset_cmd >(wrap_grammar< reset_cmd_parser< clsm::iter_t > >(enable_indexed));
			reg_cmd< save_cmd >(wrap_grammar< save_cmd_parser< clsm::iter_t > >(enable_indexed));
			reg_cmd< load_cmd >(wrap_grammar< load_cmd_parser< clsm::iter_t > >(enable_indexed));

			auto quit_str = context< paramtree_editor >().quit_str;
			reg_cmd< quit_cmd >([quit_str]()->qi::rule< clsm::iter_t, quit_cmd(), clsm::skip_t >{ return qi::lit(quit_str)[phx::nothing]; });
		}

		bool base_node_state::at_leaf_node() const
		{
			return context< paramtree_editor >().nav.at_leaf();
		}

		struct schema_nav_visitor:
			public boost::static_visitor < sch_node >
		{
			schema_nav_visitor(path::component const& cmp):
				cmp_{ cmp }
			{}

			result_type operator() (composite_sch_node const& cn) const
			{
				assert(cmp_.type() == path::ComponentType::CompositeChild);
				auto children = cn.children();
				return children.at(cmp_.as_child());
			}

			result_type operator() (list_sch_node const& ln) const
			{
				assert(cmp_.type() == path::ComponentType::ListItem);
				auto items = ln.entries();
				return items[cmp_.as_index()];
			}

			template < typename T >
			result_type operator() (T const&) const
			{
				throw std::runtime_error("wrong");
			}

			path::component const& cmp_;
		};

		sch_node base_node_state::schema_node_at(path const& pth) const
		{
			auto const& ed = context< paramtree_editor >();
/*			auto nd = ed.schema;
			for(auto const& cmp : pth)
			{
				schema_nav_visitor vis{ cmp };
				nd = nd.apply_visitor(vis);
			}
			return nd;
			*/
			auto nav = ed.nav[pth];
			return nav.tree_->node_attribs(nav.pos_.nd).schema;
		}

		sch_node base_node_state::current_schema_node() const
		{
			auto const& ed = context< paramtree_editor >();
			return schema_node_at(ed.nav.where());
		}
		
		navigator base_node_state::nav_to_node(nav_component const& nv)
		{
			auto& editor = context< paramtree_editor >();
			auto cur_nav = editor.nav;
// TODO:			acc.set_match_method(param_accessor::Match::Anywhere, param_accessor::MatchComparison::PreferBeginningOrExact);

			if(nv.nav_index)
			{
				if(at_leaf_node())
				{
					editor.output("cannot use index navigation at terminal node");
					return{};
				}

				unsigned int index = *nv.nav_index - 1;
				auto nav = cur_nav[index];
				if(nav)
				{
					return nav;
				}
				else
				{
					editor.output("index does not exist");
					return{};
				}

/*				auto vnode = cur_nav.get();
				if(vnode.is_composite())
				{
					auto children = cur_nav.child_names();
					if(index >= children.size())
					{
						editor.output("invalid nav index");
						return{};
					}

					return cur_nav[children[index]];
				}
				else if(vnode.is_list())
				{
					if(index >= cur_nav.list_num_items())
					{
						editor.output("invalid nav index");
						return{};
					}

					return cur_nav[index];
				}
				assert(false);
*/			}
			else if(nv.dir)
			{
				switch(*nv.dir)
				{
					case nav_cmd::Direction::Up:
					if(nv.search_element)
					{
/*						auto path = acc.find_path_ancestor(nv.search_element->term);
						if(!path)
						{
							std::stringstream ss;
							ss << "no ancestor \'" << nv.search_element->term << "\'";
							editor.output(ss.str());
							return{};
						}
						return path;
*/
						return{};// temp
					}
					else
					{
						if(!cur_nav.is_parent())
						{
							editor.output("no parent");
							return{};
						}
						return cur_nav.parent();
					}
					break;

					case nav_cmd::Direction::Down:
					{
						auto find_results = cur_nav.find_descendent(nv.search_element->term);
						if(!find_results.by_location.empty())
						{
							auto const& first = find_results.by_location.begin()->second;
							if(first.size() == 1)
							{
								return cur_nav[first.front()];
							}
						}

						return{};

/*						auto path = acc.find_path_descendent(nv.search_element->term, nv.search_element->rpt_indices);
						if(!path)
						{
							std::stringstream ss;
							ss << "no descendent \'" << nv.search_element->term << "\', or ambiguous";
							editor.output(ss.str());
							return{};
						}
						return path;
*/					}
					break;

					default:
					throw std::runtime_error{ "invalid nav direction" };
				}
			}
/* TODO:			else
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
*/		
			return{};
		}

		std::string base_node_state::format_value(value val)
		{
			//			typedef std::back_insert_iterator< std::string > sink_type;
			//			sink_type sink(dest);
			//			karma::generate(sink, grammar, value(s));

			std::stringstream ss;
			ss << "<b>";

			value_emitter< karma::ostream_iterator< char > > em;
			ss << karma::format(em, val);
			if(!ss)
			{
				ss.clear();
				ss << "!! output failure !!";
				return ss.str();
			}

			ss << "</b>";
			return ss.str();
		}

		struct node_type_name_visitor:
			public boost::static_visitor < std::string >
		{
			result_type operator() (boolean_sch_node const& nd)
			{
				// TODO: Hack
				if(!nd.ptr())
				{
					return "null";
				}

				return "boolean";
			}

			result_type operator() (integer_sch_node const&)
			{
				return "integer";
			}

			result_type operator() (realnum_sch_node const&)
			{
				return "real";
			}

			result_type operator() (string_sch_node const&)
			{
				return "string";
			}

			result_type operator() (enum_sch_node const&)
			{
				return "enum";
			}

			result_type operator() (list_sch_node const&)
			{
				return "list";
			}

			result_type operator() (composite_sch_node const&)
			{
				return "composite";
			}

			result_type operator() (conditional_sch_node const&)
			{
				return "conditional";
			}
		};

		std::string base_node_state::format_type_info(sch_node const& sch)
		{
			std::stringstream ss;
			node_type_name_visitor vis{};
			ss << "[" << sch.apply_visitor(vis) << "]";
			return ss.str();
		}

		struct node_constraints_visitor:
			public boost::static_visitor < std::string >
		{
			result_type operator() (boolean_sch_node const& sn)
			{
				return "";
			}

			result_type operator() (integer_sch_node const& sn)
			{
				std::stringstream ss;
				auto has_min = (bool)sn.min();
				auto has_max = (bool)sn.max();
				if(has_min && has_max)
				{
					auto min = *sn.min();
					auto max = *sn.max();
					if(min != max)
					{
						ss << min << "-" << max;
					}
					else
					{
						ss << min;
					}
				}
				else if(has_min)
				{
					ss << *sn.min() << "+";
				}
				else if(has_max)
				{
					ss << *sn.max() << "-";
				}
				auto str = ss.str();
				return str.empty() ? str : ("(" + str + ")");
			}

			result_type operator() (realnum_sch_node const& sn)
			{
				std::stringstream ss;
				auto has_min = (bool)sn.min();
				auto has_max = (bool)sn.max();
				if(has_min && has_max)
				{
					auto min = *sn.min();
					auto max = *sn.max();
					if(min != max)
					{
						ss << min << "-" << max;
					}
					else
					{
						ss << min;
					}
				}
				else if(has_min)
				{
					ss << *sn.min() << "+";
				}
				else if(has_max)
				{
					ss << *sn.max() << "-";
				}
				auto str = ss.str();
				return str.empty() ? str : ("(" + str + ")");
			}

			result_type operator() (string_sch_node const&)
			{
				return "";	// TODO:
			}

			result_type operator() (enum_sch_node const& sn)
			{
				std::stringstream ss;
				auto has_min = sn.minsel() > 0;
				auto has_max = (bool)sn.maxsel();
				if(has_min && has_max)
				{
					auto min = sn.minsel();
					auto max = *sn.maxsel();
					if(min != max)
					{
						ss << min << "-" << max << " ";
					}
					else
					{
						ss << min << " ";
					}
				}
				else if(has_min)
				{
					ss << sn.minsel() << "+ ";
				}
				else if(has_max)
				{
					ss << *sn.maxsel() << "- ";
				}

				ss << "from { ";
				auto vals = sn.enum_values_str();
				for(auto const& v : vals)
				{
					ss << v << " ";
				}
				ss << "}";
				return ss.str();
			}

			result_type operator() (list_sch_node const& sn)
			{
				std::stringstream ss;
				auto has_min = sn.minentries() > 0;
				auto has_max = (bool)sn.maxentries();
				if(has_min && has_max)
				{
					auto min = sn.minentries();
					auto max = *sn.maxentries();
					if(min != max)
					{
						ss << min << "-" << max;
					}
					else
					{
						ss << min;
					}
				}
				else if(has_min)
				{
					ss << sn.minentries() << "+";
				}
				else if(has_max)
				{
					ss << *sn.maxentries() << "-";
				}
				auto str = ss.str();
				return str.empty() ? str : ("(" + str + ")");
			}

			result_type operator() (composite_sch_node const&)
			{
				return "";
			}

			result_type operator() (conditional_sch_node const&)
			{
				return "";
			}
		};

		std::string base_node_state::format_constraints(sch_node const& sch)
		{
			node_constraints_visitor vis;
			return sch.apply_visitor(vis);
		}

		sc::result base_node_state::react(clsm::ev_cmd< nav_cmd > const& cmd)
		{
			auto new_nav = nav_to_node(cmd);
			if(new_nav)
			{
				context< paramtree_editor >().nav = new_nav;
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
			auto nav = editor.nav;

			if(cmd.nav)
			{
				nav = nav_to_node(*cmd.nav);
				if(!nav)
				{
					return;// todo: false;
				}
			}
#if 0
			std::function< void(navigator, std::string, unsigned int, unsigned int) > rec_fn;
			rec_fn = [this, &editor, &cmd, &rec_fn](navigator nav, std::string prefix,  int depth, unsigned int indent)
			{
				std::stringstream output;

				for(unsigned int i = 0; i < indent; ++i)
				{
					output << " ";
				}

				if(!prefix.empty())
				{
					output << prefix << ": ";
				}

				if(nav.at_leaf())
				{
					output << format_value(nav.get().as_value());
				}

				if(cmd.type_info)
				{
					if(nav.at_leaf())
					{
						output << " ";
					}

					auto sch = schema_node_at(nav.where());
					output << format_type_info(sch);
					output << " ";
					output << format_constraints(sch);
				}

				editor.output(output.str());

				auto max_depth = cmd.depth ? cmd.depth->value : 1u;
				if(!nav.at_leaf() && depth < max_depth)
				{
					auto vnode = nav.get();
					if(vnode.is_composite())
					{
						auto children = vnode.as_composite();
						for(auto const& entry : children)
						{
							auto const& name = entry.first;
							rec_fn(nav[name], name, depth + 1, indent + 2);
						}
					}
					else if(vnode.is_list())
					{
						auto items = vnode.as_list();
						for(size_t idx = 0; idx < items.size(); ++idx)
						{
							rec_fn(nav[idx], std::to_string(idx + 1) + ". ", depth + 1, indent + 2);
						}
					}
				}
			};
			
			rec_fn(nav, "", 0, 0);
#endif

			auto node = nav.get_ref().nd;
				//acc.node_at(path);

			unsigned int depth = 0;
			unsigned int max_depth = cmd.depth ? cmd.depth->value : 1u;
			unsigned int indent = 0;
			unsigned int index_indent = 3;	// indent for 'x. ' at far left

			auto pre_op = [&](sd_tree::node_descriptor n)
			{
				if(depth <= max_depth)
				{
					auto& tree = editor.data;

					std::stringstream output;

					auto sibling_idx = tree.position_in_siblings(n);
					auto local_indent = indent;

					auto parent = tree.get_parent(n);
					if(parent.second && parent.first == editor.nav.pos_.nd)
					{
						// Index immediate children
						output << "<b><font color=\"blue\">" << (sibling_idx + 1) << ". " << "</font></b>";
//						indent += 3;
					}
					else
					{
						local_indent += index_indent;
					}

					for(unsigned int i = 0; i < local_indent; ++i)
					{
						output << "&nbsp;";
					}

					if(tree.in_edge(n).second)
					{
						auto const& edge = tree[tree.in_edge(n).first];
						switch(edge.type)
						{
							case sd_edge_attribs::ChildType::Composite:
							output << edge.child_name << ": ";
							break;
							case sd_edge_attribs::ChildType::ListItem:
							output << "/" << (edge.item_idx + 1) << ": ";
							break;
							case sd_edge_attribs::ChildType::Conditional:
							// put anything here?
							break;
						}
					}

/*					auto in = tree.in_edge(n).first;
					if(tree[in].repeat_idx)
					{
						output << " /" << (sibling_idx + 1);
					}
*/
//					output << ": ";
					
					// TODO: Differentiate value type and non-terminal type that just doesn't currently have any children
//					if(is_leaf_type(tree[n].type))
					if(tree[n].data.is_leaf())//tree.is_leaf(n) && tree[n].data)
					{
						auto vnode = tree[n].data;
						output << format_value(vnode.as_value());
						output << " ";
					}

					if(cmd.type_info)
					{
						output << "<i>";
						output << format_type_info(tree[n].schema);
						output << " ";
						/*						auto path = pt.node_qpath(n);
												auto acc_copy = acc;
												acc_copy.move_to(path);
												*/
						output << format_constraints(tree[n].schema);//(*editor.provider)[path.unindexed()](acc_copy));
						output << "</i>";
					}

					editor.output(output.str());
				}
				++depth;
				indent += 2;
			};

			auto post_op = [&](sd_tree::node_descriptor n)
			{
				--depth;
				indent -= 2;
				if(depth == 1)
				{
//					indent -= 3;
				}
			};

			editor.output("");
			wb::gtree::depth_first_traversal(
				editor.data,
				node,
				pre_op,
				wb::gtree::null_op{},
				post_op
				);
		}

		void base_node_state::on_setvalue(clsm::ev_cmd< setvalue_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			if(cmd.nav)
			{
				nav = nav_to_node(*cmd.nav);
				if(!nav)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}

			auto sch = schema_node_at(nav.where());
				//(*editor.provider)[path.unindexed()](acc);

			value_node vnode_tmp;
			vnode_tmp = cmd.val;
			bool ok = fits_schema(vnode_tmp, sch);
			if(!ok)
			{
				editor.output("invalid value");
				return;// false;
			}

			// TODO: hack
			if(vnode_tmp.is_enum())
			{
				// TODO: Feel like the data component should not be stored as part of the value node, only the id string
				// id string -> data association is contained within the schema node.
				// seems reasonable to not duplicate this, and to have to refer back to the schema node when
				// we need the associated data value - ie. in an sd_tree/navigator::get_node_value_as<>() method
				auto vals = vnode_tmp.as_enum();
				auto en_sch = sch.get_as< enum_sch_node >();
				for(auto& elem : vals)
				{
					elem.data = en_sch.get_data_from_id_string(elem.str);
				}
				vnode_tmp = vals;
			}

			// TODO: shouldn't need to repeat lookup
//			pd.value = cmd.val;
			auto vnode = nav.get();
			vnode = vnode_tmp.as_value();//cmd.val;

			// TODO: Overkill
			editor.reload_pt_schema();

			editor.output("value set");
		}

		void base_node_state::on_reset(clsm::ev_cmd< reset_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			if(cmd.nav)
			{
				nav = nav_to_node(*cmd.nav);
				if(!nav)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}

			reset_node(nav.get_ref(), editor.data);
			// TODO: nav position

			editor.output("value reset");
		}

		void base_node_state::on_save(clsm::ev_cmd< save_cmd > const& cmd)
		{
			auto const& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			if(cmd.nav)
			{
				nav = nav_to_node(*cmd.nav);
				if(!nav)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}

			//			fs::path filepath{ cmd.filename };
			std::ofstream file{ cmd.filename };
			if(!file.is_open())
			{
				editor.output("invalid filename");
				return;// false;
			}

			auto as_yaml = export_yaml(*nav.tree_, nav.get_ref());
			file << YAML::Dump(as_yaml);

			editor.output("subtree saved");
		}

		void base_node_state::on_load(clsm::ev_cmd< load_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto nav = editor.nav;

			if(cmd.nav)
			{
				nav = nav_to_node(*cmd.nav);
				if(!nav)
				{
					editor.output("invalid path specified");
					return;// false;
				}
			}

			auto as_yaml = YAML::LoadFile(cmd.filename);
			if(as_yaml.IsNull())
			{
				editor.output("invalid filename/file");
				return;// false;
			}

/*			auto vnode = nav.get();
			// Need to modify the internal variant that is already being pointed to by vnode
			// **NOT reassign the pointer only**
			vnode.assign(import_yaml(as_yaml));

			editor.reload_pt_schema();
*/
			if(import_yaml(as_yaml, editor.data, nav.get_ref()))
			{
				// TODO: if load into node which is above current location, current location may cease to exist,
				// in which case need to move the accessor, and transition to node_type_choice
				editor.output("subtree loaded");
			}
			else
			{
				editor.output("import failed");
			}
		}

	}
}




