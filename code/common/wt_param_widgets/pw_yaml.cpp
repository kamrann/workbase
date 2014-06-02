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
		"container",
	};

	std::map< std::string, prm::ParamType > const convert< prm::ParamType >::mapping_ = {
		{ "bool", prm::ParamType::Boolean },
		{ "int", prm::ParamType::Integer },
		{ "real", prm::ParamType::RealNumber },
		{ "string", prm::ParamType::String },
		{ "enum", prm::ParamType::Enumeration },
		{ "vector2", prm::ParamType::Vector2 },
		//{ "vector3", prm::ParamType::Vector3 },
		{ "container", prm::ParamType::List },
	};

}

namespace prm {

	void schema_builder::push(YAML::Node const& n)
	{
		m_schema.push_back(n);
	}

	void schema_builder::reset()
	{
		m_schema = YAML::Node();
	}

	void schema_builder::add_boolean(std::string const& name, bool default_val)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::Boolean;
		n["default"] = default_val;
		push(n);
	}
	
	void schema_builder::add_integer(
		std::string const& name,
		int default_val,
		boost::optional< int > min_val,
		boost::optional< int > max_val)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::Integer;
		YAML::Node constraints;
		if(min_val)
		{
			constraints["min"] = *min_val;
		}
		if(max_val)
		{
			constraints["max"] = *max_val;
		}
		if(constraints)
		{
			n["constraints"] = constraints;
		}
		n["default"] = default_val;
		push(n);
	}

	void schema_builder::add_real(
		std::string const& name,
		double default_val,
		boost::optional< double > min_val,
		boost::optional< double > max_val)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::RealNumber;
		YAML::Node constraints;
		if(min_val)
		{
			constraints["min"] = *min_val;
		}
		if(max_val)
		{
			constraints["max"] = *max_val;
		}
		if(constraints)
		{
			n["constraints"] = constraints;
		}
		n["default"] = default_val;
		push(n);
	}

	void schema_builder::add_string(std::string const& name, std::string const& default_val)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::RealNumber;
		n["default"] = default_val;
		push(n);
	}

	void schema_builder::add_enum_selection(std::string const& name, std::vector< std::string > const& values)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::Enumeration;
		YAML::Node constraints;
		for(auto val : values)
		{
			constraints["values"].push_back(val);
		}
		n["constraints"] = constraints;
		push(n);
	}

	void schema_builder::add_nested_schema(std::string const& name, YAML::Node const& child)
	{
		YAML::Node n;
		n["name"] = name;
		n["type"] = prm::ParamType::List;
		n["children"] = child;
		push(n);
	}

	void schema_builder::on_update()
	{
		assert(m_schema.IsSequence());
		auto& node = m_schema[m_schema.size() - 1];
		node["update"] = "default";	// TODO:
	}

	YAML::Node schema_builder::get_schema() const
	{
		return m_schema;
	}

}



