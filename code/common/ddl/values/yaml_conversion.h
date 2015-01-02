// yaml_conversion.h

#ifndef __WB_DDL_YAML_CONVERSION_H
#define __WB_DDL_YAML_CONVERSION_H

#include "../sd_tree.h"

#include <yaml-cpp/yaml.h>


namespace ddl {

//	class value_node;
	struct sd_node_ref;

	YAML::Node export_yaml(sd_tree const& tree, sd_node_ref pos);
	bool import_yaml(YAML::Node yaml, sd_tree& tree, sd_node_ref pos);

}


#endif


