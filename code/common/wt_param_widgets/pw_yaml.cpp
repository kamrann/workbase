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
	namespace schema {

		schema_builder boolean(std::string const& name, bool default_val)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::Boolean;
			n["default"] = default_val;
			label(n, name);
			return n;
		}

		schema_builder integer(
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
			label(n, name);
			return n;
		}

		schema_builder real(
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
			label(n, name);
			return n;
		}

		schema_builder string(std::string const& name, std::string const& default_val)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::RealNumber;
			n["default"] = default_val;
			label(n, name);
			return n;
		}

		schema_builder enum_selection(std::string const& name, std::vector< std::string > const& values)
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
			label(n, name);
			return n;
		}

		schema_builder random(std::string const& name, double default_val, boost::optional< double > min_val, boost::optional< double > max_val, boost::optional< double > default_min, boost::optional< double > default_max)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::Random;
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
			if(default_min)
			{
				n["default_min"] = *default_min;
			}
			if(default_max)
			{
				n["default_max"] = *default_max;
			}
			label(n, name);
			return n;
		}

		schema_builder list(std::string const& name)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::List;
			border(n);
			return n;
		}

		void append(schema_builder& schema, schema_builder const& child)
		{
			assert(schema["type"].as< ParamType >() == ParamType::List);
			schema["children"].push_back(child);
		}

		void layout_vertical(schema_builder& schema)
		{
			assert(schema["type"].as< ParamType >() == ParamType::List);
			schema["visual"]["layout"] = container_par_wgt::Layout::Vertical;
		}

		void layout_horizontal(schema_builder& schema)
		{
			assert(schema["type"].as< ParamType >() == ParamType::List);
			schema["visual"]["layout"] = container_par_wgt::Layout::Horizontal;
		}

		void label(schema_builder& sb, std::string const& text)
		{
			sb["visual"]["label"] = text;
		}

		void unlabel(schema_builder& sb)
		{
			sb["visual"].remove("label");
		}

		void border(schema_builder& sb, boost::optional< std::string > label_text)
		{
			sb["visual"]["border"] = true;
			if(label_text)
			{
				label(sb, *label_text);
			}
		}

		void unborder(schema_builder& sb)
		{
			sb["visual"].remove("border");
		}

		void on_update(schema_builder& sb)
		{
			sb["update"] = "default";	// TODO:
		}

	}


	YAML::Node find_value(YAML::Node const& node, std::string const& name)
	{
		if(node.IsMap())
		{
			if(node[name])
			{
				return node[name];
			}
			for(auto entry : node)
			{
				auto val_node = entry.second;
				auto result = find_value(val_node, name);
				if(!result.IsNull())
				{
					return result;
				}
			}
		}

		return YAML::Node();
	}

}


