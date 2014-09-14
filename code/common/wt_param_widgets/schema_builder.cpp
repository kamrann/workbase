// schema=builder.cpp

#include "schema_builder.h"
#include "container_par_wgt.h"
#include "pw_yaml.h"


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
			n["type"] = prm::ParamType::String;
			n["default"] = default_val;
			label(n, name);
			return n;
		}

		schema_builder enum_selection(std::string const& name, std::vector< std::pair< std::string, std::string > > const& values, size_t min_sel, size_t max_sel)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::Enumeration;
			YAML::Node constraints;
			for(auto val : values)
			{
				YAML::Node enum_node;
				enum_node["id"] = val.first;
				enum_node["label"] = val.second;
				constraints["values"].push_back(enum_node);
			}
			constraints["minsel"] = min_sel;
			constraints["maxsel"] = max_sel;
			n["constraints"] = constraints;
			label(n, name);
			return n;
		}

		schema_builder enum_selection(std::string const& name, std::vector< std::string > const& labels, size_t min_sel, size_t max_sel)
		{
			std::vector< std::pair< std::string, std::string > > values(labels.size());
			std::transform(begin(labels), end(labels), begin(values), [](std::string const& label)
			{
				return std::make_pair(label, label);
			});
			return enum_selection(name, values, min_sel, max_sel);
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

		schema_builder repeating_list(std::string const& name, std::string const& contents, size_t min_count, size_t max_count)
		{
			YAML::Node n;
			n["name"] = name;
			n["type"] = prm::ParamType::Repeat;
			YAML::Node constraints;
			constraints["min_count"] = min_count;
			constraints["max_count"] = max_count;
			n["constraints"] = constraints;
			n["contents"] = contents;
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

		void enable_import(schema_builder& schema, std::string const& filter)
		{
/*			assert(
				schema["type"].as< ParamType >() == ParamType::List ||
				schema["type"].as< ParamType >() == ParamType::Repeat
				);
*/			schema["import"] = filter;
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

		void readonly(schema_builder& sb, bool ro)
		{
			sb["readonly"] = ro;
		}

		void trigger(schema_builder& sb, /* todo, */ std::string const& event_name)
		{
			sb["trigger"].push_back(event_name);
		}

		// TODO: could add argument to specify either "reset" or "retain_if_possible", so that previous value
		// retention behaviour can be event specific
		void update_on(schema_builder& sb, std::string const& event_name)
		{
			sb["update on"].push_back(event_name);
		}

		schema_builder last(schema_builder& sb)
		{
			assert(sb["type"] && sb["type"].as< ParamType >() == ParamType::List &&
				sb["children"] && sb["children"].IsSequence() && sb["children"].size() > 0);

			return sb["children"][sb["children"].size() - 1];
		}

	}
}


