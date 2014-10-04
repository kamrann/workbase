// pw_yaml.h

#ifndef __WB_PARAM_WIDGET_YAML_H
#define __WB_PARAM_WIDGET_YAML_H

#include "container_par_wgt.h"

#include <yaml-cpp/yaml.h>


namespace YAML {

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


#endif


