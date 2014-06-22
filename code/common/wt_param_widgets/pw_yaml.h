// pw_yaml.h

#ifndef __WB_PARAM_WIDGET_YAML_H
#define __WB_PARAM_WIDGET_YAML_H

#include "params/param.h"
#include "container_par_wgt.h"

#include <yaml-cpp/yaml.h>

#include <boost/variant/get.hpp>


namespace YAML {

	template <>
	struct convert< prm::ParamType >
	{
		static Node encode(prm::ParamType const& rhs)
		{
			return Node(names_[rhs]);
		}

		static bool decode(Node const& node, prm::ParamType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::string const names_[prm::ParamType::Count];
		static std::map< std::string, prm::ParamType > const mapping_;
	};

	template <>
	struct convert< prm::enum_param_val >
	{
		static Node encode(prm::enum_param_val const& rhs)
		{
			return Node(boost::any_cast< std::string >(rhs.contents));
		}

		static bool decode(Node const& node, prm::enum_param_val& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			rhs = prm::enum_param_val();
			rhs.contents = node.Scalar();
			return true;
		}
	};

	template <>
	struct convert< prm::vec2 >
	{
		static Node encode(prm::vec2 const& rhs)
		{
			Node n;
			n[0] = rhs[0];
			n[1] = rhs[1];
			return n;
		}

		static bool decode(Node const& node, prm::vec2& rhs)
		{
			if(!node.IsSequence())
			{
				return false;
			}

			if(node.size() != 2)
			{
				return false;
			}

			rhs[0] = node[0].as< double >();
			rhs[1] = node[1].as< double >();
			return true;
		}
	};

	template <>
	struct convert< prm::random >
	{
		static Node encode(prm::random const& rhs)
		{
			Node n;
			n[0] = rhs.is_fixed;
			if(rhs.is_fixed)
			{
				n[1] = boost::get< double >(rhs.range);
			}
			else
			{
				std::tie(n[1][0], n[1][1]) = boost::get< std::pair< double, double > >(rhs.range);
			}
			// TODO: distribution
			return n;
		}

		static bool decode(Node const& node, prm::random& rhs)
		{
			assert(node.IsSequence() && node.size() == 2); // todo: dist
			rhs.is_fixed = node[0].as< bool >();
			if(rhs.is_fixed)
			{
				rhs.range = node[1].as< double >();
			}
			else
			{
				rhs.range = std::make_pair(node[1][0].as< double >(), node[1][1].as< double >());
			}
			// TODO: dist
			return true;
		}
	};

	template <>
	struct convert< prm::container_par_wgt::Layout >
	{
		static Node encode(prm::container_par_wgt::Layout const& rhs)
		{
			return Node(names_[rhs]);
		}

		static bool decode(Node const& node, prm::container_par_wgt::Layout& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::string const names_[prm::container_par_wgt::Layout::Count];
		static std::map< std::string, prm::container_par_wgt::Layout > const mapping_;
	};

}


namespace prm {

	namespace schema
	{
		typedef YAML::Node schema_builder;

		schema_builder boolean(std::string const& name, bool default_val);
		schema_builder integer(std::string const& name, int default_val, boost::optional< int > min_val = boost::optional< int >(), boost::optional< int > max_val = boost::optional< int >());
		schema_builder real(std::string const& name, double default_val, boost::optional< double > min_val = boost::optional< double >(), boost::optional< double > max_val = boost::optional< double >());
		schema_builder string(std::string const& name, std::string const& default_val = "");
		schema_builder enum_selection(std::string const& name, std::vector< std::string > const& values);
		schema_builder random(std::string const& name, double default_val, boost::optional< double > min_val, boost::optional< double > max_val, boost::optional< double > default_min = boost::none, boost::optional< double > default_max = boost::none);
		schema_builder list(std::string const& name);

		void append(schema_builder& schema, schema_builder const& child);
		void layout_vertical(schema_builder& schema);
		void layout_horizontal(schema_builder& schema);

		void label(schema_builder& sb, std::string const& text);
		void unlabel(schema_builder& sb);
		void border(schema_builder& sb, boost::optional< std::string > label_text = boost::none);
		void unborder(schema_builder& sb);
		void on_update(schema_builder& sb);
	}

	// Used only for param value YAML	
	YAML::Node find_value(YAML::Node const& node, std::string const& name);

}



#endif


