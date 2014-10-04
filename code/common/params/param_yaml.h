// param_yaml.h

#ifndef __WB_PARAM_YAML_H
#define __WB_PARAM_YAML_H

#include "param.h"
#include "unspecified.h"

#include <yaml-cpp/yaml.h>

#include <boost/variant/get.hpp>


namespace YAML {

	template <>
	struct convert< prm::ParamType >
	{
		static Node encode(prm::ParamType const& rhs)
		{
			return Node(prm::ParamTypeNameMap.left.at(rhs));
		}

		static bool decode(Node const& node, prm::ParamType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = prm::ParamTypeNameMap.right.find(node.Scalar());
			if(it == prm::ParamTypeNameMap.right.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}
	};
#if 0
	template <>
	struct convert< prm::enum_param_val >
	{
		static Node encode(prm::enum_param_val const& rhs)
		{
//			return Node(boost::any_cast< std::vector< std::string > >(rhs.contents));
			return Node(rhs);
		}

		static bool decode(Node const& node, prm::enum_param_val& rhs)
		{
			if(!node.IsSequence())//IsScalar())
			{
				return false;
			}

			rhs = prm::enum_param_val();
			rhs/*.contents*/ = node.as< std::vector< std::string > >();//node.Scalar();
			return true;
		}
	};
#endif
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
			n[0] = rhs.is_fixed();
			if(rhs.is_fixed())
			{
				n[1] = rhs.as_fixed();
			}
			else
			{
				std::tie(n[1][0], n[1][1]) = rhs.as_range();
			}
			// TODO: distribution
			return n;
		}

		static bool decode(Node const& node, prm::random& rhs)
		{
			if(!(node.IsSequence() && node.size() == 2)) // todo: dist
			{
				return false;
			}

			auto is_fixed = node[0].as< bool >();
			if(is_fixed)
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
	struct convert < prm::is_unspecified_yaml >
	{
		static Node encode(prm::is_unspecified_yaml const& rhs)
		{
			throw std::exception("is_unspecified: encode() shouldn't be called!");
		}

		static bool decode(Node const& node, prm::is_unspecified_yaml& rhs)
		{
			// Should only be called for an enum_par_wgt, with single selection setup
			if(!node.IsSequence() || node.size() != 1 || !node[0].IsScalar())
			{
				return false;
			}

			rhs.val = node[0].Scalar() == prm::unspecified;
			return true;
		}
	};

}


#endif


