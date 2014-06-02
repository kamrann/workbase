// pw_yaml.h

#ifndef __WB_PARAM_WIDGET_YAML_H
#define __WB_PARAM_WIDGET_YAML_H

#include "params/param.h"

#include <yaml-cpp/yaml.h>


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

}


namespace prm {

	class schema_builder
	{
	public:
		void reset();

		void add_boolean(std::string const& name, bool default_val);
		void add_integer(std::string const& name, int default_val, boost::optional< int > min_val = boost::optional< int >(), boost::optional< int > max_val = boost::optional< int >());
		void add_real(std::string const& name, double default_val, boost::optional< double > min_val = boost::optional< double >(), boost::optional< double > max_val = boost::optional< double >());
		void add_string(std::string const& name, std::string const& default_val = "");
		void add_enum_selection(std::string const& name, std::vector< std::string > const& values);
		void add_nested_schema(std::string const& name, YAML::Node const& child);

		void on_update();

		YAML::Node get_schema() const;

	private:
		void push(YAML::Node const& n);

	private:
		YAML::Node m_schema;
	};

}



#endif


