// cmdline.cpp

#include "cmdline.h"
#include "cmd_parser.h"
#include "schema.h"

#include "value_emitter.h"

#include "util/gtree/generic_tree_depthfirst.hpp"

#include <boost/variant/static_visitor.hpp>

#include <fstream>
//#include <filesystem>


//namespace fs = std::tr2::sys;

namespace prm {

	cmdline_processor::cmdline_processor(
		schema::schema_provider_map_handle provider,
		param_tree& pt):
		m_provider{ std::move(provider) },
		m_pt{ pt },
		m_accessor{ &m_pt }
	{}

	class cmd_visitor: public boost::static_visitor < bool >
	{
	public:
		cmd_visitor(cmdline_processor& _prc, std::ostream& _os):
			prc(_prc),
			os(_os)
		{}

		inline qualified_path nav_cmd_path(nav_cmd const& cmd) const
		{
			auto acc = prc.m_accessor;
			acc.set_match_method(param_accessor::Match::Anywhere, param_accessor::MatchComparison::PreferBeginningOrExact);

			if(cmd.nav_index)
			{
				if(is_leaf_type(prc.m_accessor.param_here().type))
				{
					os << "cannot use index navigation at terminal node" << std::endl;
					return{};
				}

				unsigned int index = *cmd.nav_index - 1;
				auto children = prc.m_accessor.children();
				if(index >= children.size())
				{
					os << "invalid nav index" << std::endl;
					return{};
				}

				auto it = std::next(std::begin(children), index);
				return *it;
			}
			else if(cmd.dir)
			{
				switch(*cmd.dir)
				{
					case nav_cmd::Direction::Up:
					if(cmd.search_element)
					{
						auto path = acc.find_path_ancestor(cmd.search_element->term);
						if(!path)
						{
							os << "no ancestor \'" << cmd.search_element->term << "\'" << std::endl;
							return{};
						}
						return path;
					}
					else
					{
						auto path = acc.where();
						if(path.size() == 1)
						{
							os << "no parent" << std::endl;
							return{};
						}
						path.pop();
						return path;
					}
					break;

					case nav_cmd::Direction::Down:
					{
						auto path = acc.find_path_descendent(cmd.search_element->term, cmd.search_element->rpt_indices);
						if(!path)
						{
							os << "no descendent \'" << cmd.search_element->term << "\', or ambiguous" << std::endl;
							return{};
						}
						return path;
					}
					break;
				}
			}
			else
			{
				// Undirected nav
				auto path = acc.find_path(cmd.search_element->term, cmd.search_element->rpt_indices);
				if(!path)
				{
					os << "no node \'" << cmd.search_element->term << "\', or ambiguous" << std::endl;
					return{};
				}
				return path;
			}
		}

		inline std::ostream& output_value(param val)
		{
//			typedef std::back_insert_iterator< std::string > sink_type;
//			sink_type sink(dest);
//			karma::generate(sink, grammar, value(s));

			value_emitter< karma::ostream_iterator< char > > em;
			os << karma::format(em, val);

			if(!os)
			{
				os.clear();
				os << std::endl << "!! output failure !!" << std::endl;
			}

			return os;
		}

		inline std::ostream& output_constraints(schema::schema_node sch)
		{
			auto type = sch["type"].as< ParamType >();
			switch(type)
			{
				case ParamType::Enumeration:
				{
					os << "in { ";
					for(auto node : sch["constraints"]["values"])
					{
						// TODO: shouldn't this be name rather than id? likewise for parsing/emitting
						os << node["id"] << " ";
					}
					os << "}";
				}
				break;
			}

			return os;
		}

		inline std::ostream& output_type_info(param_tree::param_data const& pd)
		{
			os << "[" << ParamTypeNameMap.left.at(pd.type) << "]";
			return os;
		}

		inline result_type operator() (nav_cmd const& cmd)
		{
			qualified_path new_path = nav_cmd_path(cmd);
			if(new_path)
			{
				prc.m_accessor.move_to(new_path);
				return true;
			}
			else
			{
				return false;
			}
		}

		inline result_type operator() (list_cmd const& cmd)
		{
			qualified_path path;
			param_tree::param_data pd;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return false;
				}
				pd = prc.m_accessor.find_param(path);
			}
			else
			{
				path = prc.m_accessor.where();
				pd = prc.m_accessor.param_here();
			}

			auto node = prc.m_accessor.node_at(path);

			unsigned int depth = 0;
			unsigned int max_depth = cmd.depth ? cmd.depth->value : 1u;
			unsigned int indent = 0;

			auto pre_op = [this, &depth, &indent, &cmd, max_depth](param_tree::tree_t::node_descriptor n)
			{
				if(depth <= max_depth)
				{
					auto& pt = prc.m_pt;
					auto& tree = pt.tree();

					for(unsigned int i = 0; i < indent; ++i)
					{
						std::cout << " ";
					}

					auto sibling_idx = tree.position_in_siblings(n);
					if(depth == 1)
					{
						// Index immediate children
						std::cout << (sibling_idx + 1) << ". ";

						indent += 3;
					}
					
					std::cout << tree[n].name;

					auto in = tree.in_edge(n).first;
					if(tree[in].repeat_idx)
					{
						std::cout << " /" << (sibling_idx + 1);
					}

					std::cout << ": ";
					if(is_leaf_type(tree[n].type))
					{
						output_value(*tree[n].value);
					}

					if(cmd.type_info)
					{
						os << " ";
						output_type_info(tree[n]);
						os << " ";
						auto path = pt.node_qpath(n);
						auto acc = prc.m_accessor;
						acc.move_to(path);
						output_constraints((*prc.m_provider)[path.unindexed()](acc));
					}

					std::cout << std::endl;
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
				prc.m_pt.tree(),
				node,
				pre_op,
				wb::gtree::null_op{},
				post_op
				);
			return true;
		}

		inline result_type operator() (setvalue_cmd const& cmd)
		{
			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return false;
				}
			}
			else
			{
				path = prc.m_accessor.where();
			}

			auto& pd = prc.m_accessor.find_param(path);
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*prc.m_provider)[path.unindexed()](prc.m_accessor);

			bool ok = schema::validate_param(cmd.val, sch);
			if(!ok)
			{
				os << "invalid value" << std::endl;
				return false;
			}

			// TODO: shouldn't need to repeat lookup
			pd.value = cmd.val;

			// TODO: Overkill
			prc.reload_pt_schema();

			return true;
		}

		inline result_type operator() (repeat_add_cmd const& cmd)
		{
			auto& pr = prc.m_accessor.param_here();
			if(pr.type != ParamType::Repeat)
			{
				os << "cannot add here, not repeat param" << std::endl;
				return false;
			}

			// Make a copy of the accessor/param tree
			auto pt_copy = param_tree{ prc.m_pt };
			auto acc_copy = param_accessor{ &pt_copy };
			acc_copy.move_to(prc.m_accessor.where());

			prc.m_pt.add_repeat_instance(prc.m_accessor.node_here(), acc_copy, prc.m_provider);

			// TODO: Overkill
			prc.reload_pt_schema();

			return true;
		}

		inline result_type operator() (repeat_remove_cmd const& cmd)
		{
			auto& pr = prc.m_accessor.param_here();
			if(pr.type != ParamType::Repeat)
			{
				os << "cannot remove here, not repeat param" << std::endl;
				return false;
			}

			return prc.m_pt.remove_repeat_instance(prc.m_accessor.node_here(), cmd.index);
		}

		inline result_type operator() (save_cmd const& cmd)
		{
			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return false;
				}
			}
			else
			{
				path = prc.m_accessor.where();
			}

//			fs::path filepath{ cmd.filename };
			std::ofstream file{ cmd.filename };
			if(!file.is_open())
			{
				std::cout << "invalid filename" << std::endl;
				return false;
			}

			auto subtree_root = prc.m_accessor.node_at(path);
			auto as_yaml = prc.m_pt.convert_to_yaml(subtree_root);

			file << YAML::Dump(as_yaml);

			return true;
		}

		inline result_type operator() (load_cmd const& cmd)
		{
			qualified_path path;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return false;
				}
			}
			else
			{
				path = prc.m_accessor.where();
			}

			//			fs::path filepath{ cmd.filename };
/*			std::ifstream file{ cmd.filename };
			if(!file.is_open())
			{
				std::cout << "invalid filename" << std::endl;
				return false;
			}
*/
			auto as_yaml = YAML::LoadFile(cmd.filename);
			if(as_yaml.IsNull())
			{
				std::cout << "invalid filename/file" << std::endl;
				return false;
			}

			auto subtree_root = prc.m_accessor.node_at(path);
			auto& tree = prc.m_pt.tree();
			tree.clear_children(subtree_root);

			prc.m_pt.generate_from_yaml(as_yaml, subtree_root);

/* TODO: generate from yaml does not validate loaded params against schema...

			bool ok = schema::validate_param(cmd.val, sch);
			if(!ok)
			{
				os << "invalid value" << std::endl;
				return false;
			}
*/
			// TODO: Overkill
			prc.reload_pt_schema();

			return true;
		}

		inline result_type operator() (debug_cmd const& cmd)
		{
			prc.m_pt.dump(std::cout);
			return true;
		}

		cmdline_processor& prc;
		std::ostream& os;
	};

	std::string cmdline_processor::prompt() const
	{
		return m_accessor.where().to_string();
	}

	bool cmdline_processor::process_cmd(std::string const& cmd_str, std::ostream& out)
	{
		auto location_type = m_accessor.param_here().type;
		cmd_parser< std::string::const_iterator > parser{ is_leaf_type(location_type) };
		
		auto it = std::begin(cmd_str);
		command cmd;
		auto result = qi::phrase_parse(it, std::end(cmd_str), parser, qi::space_type{}, cmd);

		if(!result)
		{
			out << "failed" << std::endl;
			return false;
		}

		cmd_visitor vis{ *this, out };
		return boost::apply_visitor(vis, cmd);
	}

	void cmdline_processor::enter_cmd_loop(std::istream& in, std::ostream& out, std::string const& quit_str)
	{
		while(true)
		{
			out << prompt() << ": ";

			std::string input;
			std::getline(in, input);

			if(input == quit_str)
			{
				break;
			}

			process_cmd(input, out);
		}
	}

	void cmdline_processor::reload_pt_schema()
	{
		auto pos = m_accessor.where();
		auto rootname = m_pt.rootname();
		auto sch = (*m_provider)[rootname](m_accessor);
		
		// Make a deep copy of the accessor/param tree
		param_tree pt_copy{ m_pt };
		param_accessor acc_copy{ &pt_copy };
		acc_copy.move_to(pos);

		m_pt = param_tree::generate_from_schema(sch, acc_copy, m_provider);

		m_accessor = param_accessor{ &m_pt };
		m_accessor.move_to(pos);
	}

}




