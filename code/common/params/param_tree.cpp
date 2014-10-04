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

	void param_tree::generate_from_schema(schema::schema_node s, tree_t::node_descriptor node, param_accessor acc)
	{
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
				for(auto c : s["children"])
				{
					auto child_acc = acc;
					if(child_acc && !child_acc.move_relative(c["name"].as< std::string >())) // todo: Maybe allow to push a non-valid state onto the location stack,
						// so we can put an accessor into invalid state and then revert later
					{
						// This node does not exist in the existing param tree, so for this subtree, use a null param_accessor
						// so that everything will be generated from defaults
						child_acc = param_accessor{};
					}
					auto c_node = m_tree.add_node(node).second;
					generate_from_schema(c, c_node, child_acc);
				}
			}
			break;

			case ParamType::Repeat:
			{
				// TODO: respect repeat min instance count
			}
			break;
		}

		m_tree[node].type = type;
		m_tree[node].name = name;
	}

	param_tree param_tree::generate_from_schema(schema::schema_node s, param_accessor acc)
	{
		if(s.IsNull())
		{
			return param_tree{};
		}

		param_tree pt;
		auto root = pt.m_tree.create_root().first;
//		acc.move_to(qualified_path{});
		if(acc && !acc.move_to(s["name"].as< std::string >()))
		{
			acc = param_accessor{};
		}
		pt.generate_from_schema(s, root, acc);
		return pt;
	}

	param_tree param_tree::generate_from_schema(schema::schema_node s)
	{
		return generate_from_schema(s, param_accessor{});
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
				// TODO:
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
			pre_op,
			wb::gtree::null_op{},
			post_op
			);
		return yaml;
	}

}



