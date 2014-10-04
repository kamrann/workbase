// schema.cpp

#include "schema.h"
#include "param_yaml.h"

#include <boost/variant/static_visitor.hpp>


namespace prm {
	namespace schema {

		struct validator_visitor: public boost::static_visitor < bool >
		{
			validator_visitor(schema_node _sch):
				sch(_sch),
				constraints(_sch["constraints"]),
				type(_sch["type"].as< ParamType >())
			{}

			template < typename T >
			bool check_min_max(T const& val) const
			{
				if(constraints)
				{
					if(constraints["min"] && val < constraints["min"].as< T >())
					{
						return false;
					}
					if(constraints["max"] && val > constraints["max"].as< T >())
					{
						return false;
					}
				}
				return true;
			}

			inline result_type operator() (bool b) const
			{
				if(type != ParamType::Boolean)
				{
					return false;
				}
				return true;
			}

			inline result_type operator() (int i) const
			{
				if(type != ParamType::Integer)
				{
					return false;
				}
				if(!check_min_max(i))
				{
					return false;
				}
				return true;
			}

			inline result_type operator() (double d) const
			{
				if(type != ParamType::RealNumber)
				{
					return false;
				}
				if(!check_min_max(d))
				{
					return false;
				}
				return true;
			}

			inline result_type operator() (std::string const& s) const
			{
				if(type != ParamType::String)
				{
					return false;
				}
				return true;
			}

			inline result_type operator() (enum_param_val const& e) const
			{
				if(type != ParamType::Enumeration)
				{
					return false;
				}
				auto minsel = constraints["minsel"].as< size_t >();
				auto maxsel = constraints["maxsel"].as< size_t >();
				if(e.size() < minsel || e.size() > maxsel)
				{
					return false;
				}
				
				std::set< std::string > allowed_elements;
				for(auto node : constraints["values"])
				{
					allowed_elements.insert(node["id"].as< std::string >());
				}
				bool allow_unspecified = (minsel == 0 && maxsel == 1);
				if(allow_unspecified)
				{
					allowed_elements.insert(unspecified);
				}

				for(auto const& elem : e)
				{
					if(allowed_elements.find(elem) == allowed_elements.end())
					{
						return false;
					}
				}
				return true;
			}

			inline result_type operator() (vec2 const& v) const
			{
				if(type != ParamType::Vector2)
				{
					return false;
				}
				return true;
			}

			inline result_type operator() (random const& r) const
			{
				if(type != ParamType::Random)
				{
					return false;
				}
				if(r.is_fixed())
				{
					if(!check_min_max(r.as_fixed()))
					{
						return false;
					}
				}
				else
				{
					auto rg = r.as_range();
					if(!check_min_max(rg.first) || !check_min_max(rg.second))
					{
						return false;
					}
					if(rg.first >= rg.second)
					{
						return false;
					}
				}
				return true;
			}

			schema_node sch;
			schema_node constraints;
			ParamType type;
		};

		bool validate_param(param const& p, schema_node sch)
		{
			validator_visitor validator{ sch };
			return boost::apply_visitor(validator, p);
		}


#if 0
		param generate_default(schema_node sch)
		{
			param result;
			result[ParamNode::Name] = sch["name"];
			auto type = sch["type"].as< ParamType >();
			result[ParamNode::Type] = type;

			switch(type)
			{
				case ParamType::List:
				//result[ParamNode::Value] = YAML::Load("[]");
				for(auto child : sch["children"])
				{
					result[ParamNode::Value].push_back(generate_default(child));
				}
				break;

				case ParamType::Repeat:
				// TODO: respect minimum count
				result[ParamNode::Value] = YAML::Load("{}");
				break;

				case ParamType::Enumeration:
				{
					auto constraints = sch["constraints"];
					if(constraints["maxsel"].as< int >() == 1)
					{
						if(constraints["minsel"].as< int >() == 0)
						{
							result[ParamNode::Value].push_back(unspecified);
						}
						else
						{
							result[ParamNode::Value].push_back(constraints["values"][0]["id"]);
						}
					}
					else
					{
						result[ParamNode::Value] = YAML::Load("[]");
					}
				}
				break;

				default:
				result[ParamNode::Value] = sch["default"];
				break;
			}

			return result;
		}
#endif
	}
}


