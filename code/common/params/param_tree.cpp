// param_tree.cpp

#include "param_tree.h"
#include "param_accessor.h"
#include "schema.h"
#include "param_yaml.h"

#include "util/gtree/generic_tree_depthfirst.hpp"


namespace prm
{
	std::string param_tree::rootname() const
	{
		return m_tree[m_tree.get_root()].name;
	}

	param param_tree::generate_default_terminal(schema::schema_node s)
	{
		auto type = s["type"].as< ParamType >();
		switch(type)
		{
			case ParamType::Boolean:
			return s["default"].as< bool >();

			case ParamType::Integer:
			return s["default"].as< int >();

			case ParamType::RealNumber:
			return s["default"].as< double >();

			case ParamType::Enumeration:
			return s["default"].as< enum_param_val >();

			case ParamType::String:
			return s["default"].as< std::string >();

			case ParamType::Vector2:
			return s["default"].as< vec2 >();

			case ParamType::Random: // TODO: schema builder currently requires double as default
			return random{ s["default"].as< double >() };//s["default"].as< random >();

			default:
			throw std::runtime_error{ "invalid param type" };
		}
	}

	void param_tree::generate_from_schema(schema::schema_node s, tree_t::node_descriptor node, param_accessor acc, schema::schema_provider_map_handle sch_mp)
	{
/*		if(acc.m_pt == this)
		{
			throw std::runtime_error{ "shouldn't happen!" };
		}
*/
		acc.set_lock_on_failed_move(true);

		auto name = s["name"].as< std::string >();
/*		if(acc && !acc.move_relative(name)) // todo: Maybe allow to push a non-valid state onto the location stack,
									// so we can put an accessor into invalid state and then revert later
		{
			// This node does not exist in the existing param tree, so for this subtree, use a null param_accessor
			// so that everything will be generated from defaults
			acc = param_accessor{};
		}
*/
		auto type = s["type"].as< ParamType >();

		m_tree[node].type = type;
		m_tree[node].name = name;

		if(is_leaf_type(type))
		{
			// For leaf types, if a node with matching name and type is found in the given accessor, just
			// copy across its value
			if(acc && acc.where().leaf().name() == name && acc.param_here().type == type)
			{
				m_tree[node].value = acc.value_here();
			}
			else
			{
				// Not found, so use default value
				m_tree[node].value = generate_default_terminal(s);
			}
		}
		else switch(type)	// Non-leaf types
		{
			case ParamType::List:
			{
				YAML::Node children = s["children"];
				for(YAML::Node c : children)
				{
					auto child_acc = acc;
					// TODO: This test (and also the one below for repeats) is supposed to ensure that the 
					// accessor is valid for the current location. It's not ideal, since nothing to stop
					// the same name occurring at multiple nodes in the tree.
					if(child_acc && child_acc.where().leaf().name() == name)
					{
						// Attempt to move the accessor to the newly created node.
						// Failure will lock the accessor ensuring that any copies of it in recursive calls
						// will not move it from its current position (which is the position furthest down the
						// existing param tree towards the current node which actually exists).
						child_acc.move_relative(c["name"].as< std::string >());
					}
					auto c_node = m_tree.add_node(node).second;
					generate_from_schema(c, c_node, child_acc, sch_mp);
				}
			}
			break;

			case ParamType::Repeat:
			{
				auto contents_string = s["contents"].as< std::string >();
				m_tree[node].extra = repeat_extra{ contents_string };

				if(acc && acc.where().leaf().name() == name)
				{
					// Maintain any existing instances
					auto existing_instances = acc.children();
					for(auto const& p : existing_instances)
					{
						add_repeat_instance(node, acc, sch_mp);
					}
				}

				// TODO: respect repeat min instance count
			}
			break;
		}
	}

	param_tree param_tree::generate_from_schema(schema::schema_node s, param_accessor acc, schema::schema_provider_map_handle sch_mp)
	{
		if(s.IsNull())
		{
			return param_tree{};
		}

		param_tree pt;
		auto root = pt.m_tree.create_root().first;
		if(acc)
		{
			acc.set_lock_on_failed_move(true);
			acc.move_to(s["name"].as< std::string >());
		}
		pt.generate_from_schema(s, root, acc, sch_mp);
		return pt;
	}

	param_tree param_tree::generate_from_schema(schema::schema_node s, schema::schema_provider_map_handle sch_mp)
	{
		return generate_from_schema(s, param_accessor{}, sch_mp);
	}

	qualified_path param_tree::node_qpath(tree_t::node_descriptor node) const
	{
		auto qpath = qualified_path{};
		auto tree_path = m_tree.path_to_node(node);
		auto prev = tree_t::null_vertex();
		for(auto it = std::begin(tree_path); it != std::end(tree_path); prev = *it++)
		{
			auto n = *it;
			if(prev != tree_t::null_vertex() && m_tree[prev].type == ParamType::Repeat)
			{
				qpath.leaf().set_index(*m_tree[m_tree.in_edge(n).first].repeat_idx);
			}

			qpath += m_tree[n].name;
		}

		return qpath;
	}

	bool param_tree::add_repeat_instance(tree_t::node_descriptor rpt_node, param_accessor acc, schema::schema_provider_map_handle sch_mp)
	{
		auto nd_attribs = m_tree[rpt_node];
		if(nd_attribs.type != ParamType::Repeat)
		{
			return false;
		}

		// TODO: respect repeat max count
		auto inst_num = 0;
		bool first = m_tree.child_count(rpt_node) == 0;
		if(!first)
		{
			// Get the edge of the rightmost existing instance
			auto edge = m_tree.in_edge(m_tree.get_rightmost_child(rpt_node).first).first;
			// And add 1 to its instance number to generate the instance number of the new instance
			// Note: This assumes that instances cannot be repositioned once created.
			inst_num = *m_tree[edge].repeat_idx + 1;
		}
		auto instance = m_tree.add_node(rpt_node).second;
		m_tree[m_tree.in_edge(instance).first].repeat_idx = inst_num;

		auto path = node_qpath(rpt_node);
		path.leaf().set_index(inst_num);
		auto contents_name = boost::any_cast<repeat_extra>(m_tree[rpt_node].extra).contents_name;
		path += contents_name;

		acc.set_lock_on_failed_move(true);
		acc.move_to(path);
		auto sch = sch_mp->at(path.unindexed())(acc);
		generate_from_schema(sch, instance, acc, sch_mp);
		return true;
	}

	bool param_tree::remove_repeat_instance(tree_t::node_descriptor rpt_node, unsigned int index)
	{
		auto nd_attribs = m_tree[rpt_node];
		if(nd_attribs.type != ParamType::Repeat)
		{
			return false;
		}

		auto ccount = m_tree.child_count(rpt_node);
		if(index >= ccount)
		{
			return false;
		}

		auto branches = m_tree.branches(rpt_node);
		// TODO: Move gtree over from boost to std wherever possible
		auto b_it = boost::next(std::begin(branches), index);
		m_tree.remove_branch(*b_it);
		return true;
	}

	param param_tree::generate_terminal_from_yaml(YAML::Node yaml, ParamType type)
	{
		switch(type)
		{
			case ParamType::Boolean:
			return yaml.as< bool >();
			break;
			case ParamType::Integer:
			return yaml.as< int >();
			break;
			case ParamType::RealNumber:
			return yaml.as< double >();
			break;
			case ParamType::String:
			return yaml.as< std::string >();
			break;
			case ParamType::Enumeration:
			return yaml.as< enum_param_val >();
			break;
			case ParamType::Random:
			return yaml.as< random >();
			break;
			case ParamType::Vector2:
			return yaml.as< vec2 >();
			break;

			default:
			throw std::runtime_error("invalid param type");
		}
	}

	void param_tree::generate_from_yaml(YAML::Node yaml, tree_t::node_descriptor node)
	{
		m_tree[node].name = yaml[YAMLParamNode::Name].as< std::string >();
		auto type = yaml[YAMLParamNode::Type].as< ParamType >();
		m_tree[node].type = type;

		if(is_leaf_type(type))
		{
			m_tree[node].value = generate_terminal_from_yaml(yaml[YAMLParamNode::Value], type);
		}
		else switch(type)
		{
			case ParamType::List:
			{
				for(auto c : yaml[YAMLParamNode::Value])
				{
					auto c_node = m_tree.add_node(node).second;
					generate_from_yaml(c, c_node);
				}
			}
			break;

			case ParamType::Repeat:
			{
				size_t idx = 0;
				for(auto c : yaml[YAMLParamNode::Value])
				{
					// TODO: Missing content name
					// However, feel like should be possible to have a param tree without an attached schema.
					// Distinction between a list and a repeat is a schema level distinction, not data level...
					auto c_node = m_tree.add_node(node).second;
					auto c_edge = m_tree.in_edge(c_node).first;
					m_tree[c_edge].repeat_idx = idx++;
					generate_from_yaml(c, c_node);
				}
			}
			break;
		}
	}
	
	param_tree param_tree::generate_from_yaml(YAML::Node const& yaml)
	{
		if(yaml.IsNull())
		{
			return param_tree{};
		}

		param_tree pt;
		auto root = pt.m_tree.create_root().first;
		pt.generate_from_yaml(yaml, root);
		return pt;
	}

	struct cvt_variant_visitor:
		public boost::static_visitor < YAML::Node >
	{
		template < typename T >
		result_type operator() (T const& val) const
		{
			return YAML::Node(val);
		}
	};

	YAML::Node param_tree::convert_to_yaml() const
	{
		return convert_to_yaml(m_tree.get_root());
	}

	YAML::Node param_tree::convert_to_yaml(tree_t::node_descriptor node) const
	{
		YAML::Node yaml;
		std::stack< YAML::Node > stk;

		auto pre_op = [this, &stk](tree_t::node_descriptor n)
		{
			auto yaml_node = YAML::Node();
			yaml_node[YAMLParamNode::Name] = m_tree[n].name;
			yaml_node[YAMLParamNode::Type] = m_tree[n].type;
			stk.push(yaml_node);
		};

		auto post_op = [this, &stk, &yaml](tree_t::node_descriptor n)
		{
			YAML::Node yaml_node = stk.top();
			if(is_leaf_type(m_tree[n].type))
			{
				yaml_node[YAMLParamNode::Value] = boost::apply_visitor(
					cvt_variant_visitor{},
					*m_tree[n].value
					);
			}

			stk.pop();
			if(stk.empty())
			{
				yaml = yaml_node;
			}
			else
			{
				stk.top()[YAMLParamNode::Value].push_back(yaml_node);
			}
		};

		wb::gtree::depth_first_traversal(
			m_tree,
			node,
			pre_op,
			wb::gtree::null_op{},
			post_op
			);
		return yaml;
	}

	void param_tree::dump(std::ostream& os) const
	{
		int indent = 0;

		auto pre_op = [this, &os, &indent](tree_t::node_descriptor n)
		{
			for(int i = 0; i < indent; ++i)
			{
				os << "  ";
			}
			auto in = m_tree.in_edge(n).first;
			if(m_tree[in].repeat_idx)
			{
				os << "[" << *m_tree[in].repeat_idx << "] ";
			}
			os << m_tree[n].name;
			os << std::endl;
			++indent;
		};

		auto post_op = [this, &indent](tree_t::node_descriptor n)
		{
			--indent;
		};

		wb::gtree::depth_first_traversal(
			m_tree,
			pre_op,
			wb::gtree::null_op{},
			post_op
			);
	}

}



