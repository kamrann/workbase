// pw_yaml.cpp

#include "pw_yaml.h"


namespace YAML {

	std::string const convert< prm::ParamType >::names_[prm::ParamType::Count] = {
		"bool",
		"int",
		"real",
		"string",
		"enum",
		"vector2",
		//"vector3",
		"random",
		"container",
		"repeat",
	};

	std::map< std::string, prm::ParamType > const convert< prm::ParamType >::mapping_ = {
			{ "bool", prm::ParamType::Boolean },
			{ "int", prm::ParamType::Integer },
			{ "real", prm::ParamType::RealNumber },
			{ "string", prm::ParamType::String },
			{ "enum", prm::ParamType::Enumeration },
			{ "vector2", prm::ParamType::Vector2 },
			//{ "vector3", prm::ParamType::Vector3 },
			{ "random", prm::ParamType::Random },
			{ "container", prm::ParamType::List },
			{ "repeat", prm::ParamType::Repeat },
	};


	std::string const convert< prm::container_par_wgt::Layout >::names_[prm::container_par_wgt::Layout::Count] = {
		"vertical",
		"horizontal",
	};

	std::map< std::string, prm::container_par_wgt::Layout > const convert< prm::container_par_wgt::Layout >::mapping_ = {
			{ "vertical", prm::container_par_wgt::Layout::Vertical },
			{ "horizontal", prm::container_par_wgt::Layout::Horizontal },
			{ "vert", prm::container_par_wgt::Layout::Vertical },
			{ "horiz", prm::container_par_wgt::Layout::Horizontal },
			{ "v", prm::container_par_wgt::Layout::Vertical },
			{ "h", prm::container_par_wgt::Layout::Horizontal },
	};

}

namespace prm {

	param instantiate_includes(param const& node, bool recursive)
	{
		if(node.Tag().compare("!import") == 0)
		{
			auto source = node.as< std::string >();
			auto imported = YAML::LoadFile(source);
			return recursive ? instantiate_includes(imported, true) : imported;
		}
		else
		{
			auto node_type = node[ParamNode::Type].as< ParamType >();
			auto value_node = node[ParamNode::Value];

			param result;
			result[ParamNode::Name] = node[ParamNode::Name].as< std::string >();
			result[ParamNode::Type] = node_type;

			if(recursive && node_type == ParamType::List)
			{
				for(auto c : value_node)
				{
					result[ParamNode::Value].push_back(instantiate_includes(c, true));
				}
			}
			else if(recursive && node_type == ParamType::Repeat)
			{
				for(auto c : value_node)
				{
					result[ParamNode::Value][c.first.as< std::string >()] = instantiate_includes(c.second, true);
				}
			}
			else
			{
				result[ParamNode::Value] = YAML::Clone(value_node);
			}

			return result;
		}
	}

	fixed_or_random<
		double,
		boost::random::uniform_real_distribution< double >,
		boost::random::mt19937
	> extract_fixed_or_random(
		param const& node, double default_value)
	{
		if(!node.IsDefined())
		{
			return{ default_value };
		}

		try
		{
			auto val = node.as< prm::random >();

			if(val.is_fixed)
			{
				return{ boost::get< double >(val.range) };
			}
			else
			{
				auto r = boost::get< std::pair< double, double > >(val.range);
				return{ r.first, r.second };
			}
		}
		catch(...)
		{
		}

		try
		{
			auto val = node.as< double >();

			return{ val };
		}
		catch(...)
		{
		}

		throw std::exception("Invalid FixedOrRandom value");
	}

	std::set< qualified_path > construct_abs_paths(param const& p, qualified_path const& cur_path = qualified_path())
	{
		std::set< qualified_path > paths;

		if(p.IsNull())
		{
			return paths;
		}

		auto name = p[ParamNode::Name].as< std::string >();
		auto type = p[ParamNode::Type].as< prm::ParamType >();

		switch(type)
		{
			case prm::ParamType::List:
			for(auto child : p[ParamNode::Value])
			{
				auto branch_results = construct_abs_paths(child, cur_path + name);
				paths.insert(std::begin(branch_results), std::end(branch_results));
			}
			break;

			case prm::ParamType::Repeat:
			for(auto inst : p[ParamNode::Value])
			{
				auto inst_num = inst.first.as< unsigned int >();
				auto indexed_path = cur_path + name;
				indexed_path.leaf().set_index(inst_num);
				auto child = inst.second;
				auto branch_results = construct_abs_paths(child, indexed_path);
				paths.insert(std::begin(branch_results), std::end(branch_results));
			}

			default:
			break;
		}

		paths.insert(cur_path + name);
		return paths;
	}

	bool is_param_node(YAML::Node n)
	{
		return n.IsSequence() && n.size() == 3;
	}

	qualified_path resolve_id(
		std::string const& name,
		param const& p,
		qualified_path const& path,
		std::set< qualified_path > const& all_paths)
	{
		for(auto const& abs_path : all_paths)
		{
			// TODO: Currently assuming name is entirely unqualified - leaf only
			if(abs_path.leaf().name() == name)
			{
				if(//qualified_path::match_indexes(abs_path, path))
					abs_path.is_unambiguous_from(path))
				{
					// Found
					return abs_path;
				}
				// else, failed to match, so continue
			}
		}

		// TODO: For now, assuming failure to resolve is okay, and denoted by empty qualified_path
		return qualified_path();	// todo: failure
	}

	param find_value(param const& node, qualified_path const& id)//std::string const& name)
	{
/*		if(id.size() == 0)
		{
			return node;
		}

		auto const& base = id[0];
		if(node.IsMap() && node[base.name()])
		{
			auto child = node[base.name()];
			if(!base.has_index())
			{
				return find_value(child, id.tail());
			}
			else if(child.IsSequence())
			{
				return find_value(child[base.index()], id.tail());
			}
		}

		return param();
*/

		auto const& base = id.root();
		if(base.name() == node[ParamNode::Name].as< std::string >())
		{
			if(id.is_leaf())
			{
				/*///// TODO: Seems like a hack...
				if(node[ParamNode::Type].as< ParamType >() == ParamType::Repeat && base.has_index())
				{
					return node[ParamNode::Value][]
				}
				else
				/*/////
				return node[ParamNode::Value];
			}
			else switch(node[ParamNode::Type].as< ParamType >())
			{
				case ParamType::List:
				for(auto child : node[ParamNode::Value])
				{
					auto result = find_value(child, id.tail());
					if(result)
					{
						return result;
					}
				}
				return param(YAML::NodeType::Undefined);

				case ParamType::Repeat:
				{
					assert(base.has_index());
					auto idx = base.index();

					auto child = node[ParamNode::Value][idx];
					auto result = find_value(child, id.tail());
					if(result)
					{
						return result;
					}
				}
				return param(YAML::NodeType::Undefined);

				default:
//				return id.is_leaf() ? node[ParamNode::Value] : param();
				return param(YAML::NodeType::Undefined);
			}
		}
		else
		{
			return param(YAML::NodeType::Undefined);
		}

		/*
		if(id.is_leaf())
		{
			//indexes ? maybe have to pass through index argument to each find_value() call ? (when direct parent is a repeat)
			if(is_param_node(node) && node[ParamNode::Name].as< std::string >() == id.leaf().name())
			{
				return node[ParamNode::Value];
			}
			else
			{
				return param();
			}
		}

		auto const& base = id[0];
		if(node.IsMap() && node[base.name()])
		{
			auto child = node[base.name()];
			if(!base.has_index())
			{
				return find_value(child, id.tail());
			}
			else if(child.IsSequence())
			{
				return find_value(child[base.index()], id.tail());
			}
		}

		return param();
		*/
	}

}


