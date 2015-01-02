// param.cpp

#include "param.h"
#include "param_yaml.h"
#include "qualified_path.h"


namespace prm {

	bimap< ParamType, std::string > const ParamTypeNameMap = {
			{ ParamType::Boolean, "bool" },
			{ ParamType::Integer, "int" },
			{ ParamType::RealNumber, "real" },
			{ ParamType::String, "string" },
			{ ParamType::Enumeration, "enum" },
			{ ParamType::Vector2, "vector2" },
//			{ ParamType::Vector3, "vector3" },
			{ ParamType::Random, "random" },
			{ ParamType::List, "container" },
			{ ParamType::Repeat, "repeat" },
	};


	fixed_or_random<
		double,
		std::uniform_real_distribution< double >,
		std::default_random_engine
	> extract_fixed_or_random(
	prm::param const& p,
	double default_value	// TODO: this would be used if we allow 'unspecified' val for all types, not only enum
	)
	{
		// Try random
		try
		{
			auto rnd = boost::get< random >(p);
			if(rnd.is_fixed())
			{
				return{ rnd.as_fixed() };
			}
			else
			{
				return{ rnd.as_range().first, rnd.as_range().second };
			}
		}
		catch(...)
		{
		}

		// Try double
		try
		{
			auto val = boost::get< double >(p);
			return{ val };
		}
		catch(...)
		{
		}

		// Int ??

		// Fail
		throw std::runtime_error{ "param cannot be interpreted as fixed_or_random" };
	}


#if 0
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

	param find_node(param const& node, qualified_path const& id)
	{
		auto const& base = id.root();
		if(base.name() == node[ParamNode::Name].as< std::string >())
		{
			if(id.is_leaf())
			{
				return node;
			}
			else switch(node[ParamNode::Type].as< ParamType >())
			{
				case ParamType::List:
				for(auto child : node[ParamNode::Value])
				{
					auto result = find_node(child, id.tail());
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
					auto result = find_node(child, id.tail());
					if(result)
					{
						return result;
					}
				}
				return param(YAML::NodeType::Undefined);

				default:
				return param(YAML::NodeType::Undefined);
			}
		}
		else
		{
			return param(YAML::NodeType::Undefined);
		}
	}
#endif

}


