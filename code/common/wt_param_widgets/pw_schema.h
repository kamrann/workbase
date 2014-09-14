// pw_schema.h

#ifndef __WB_PARAM_WIDGET_SCHEMA_H
#define __WB_PARAM_WIDGET_SCHEMA_H

#include "params/param.h"

#include <map>
#include <memory>


namespace YAML {
	class Node;
}

namespace prm {

	class qualified_path;
	class param_accessor;

	namespace schema {

		typedef YAML::Node schema_node;

		typedef std::function< schema_node (qualified_path const&, param_accessor /*const*/ &) > schema_provider;

		typedef std::function< schema_node(param_accessor /* const */ &) > single_schema_provider;
		typedef std::map< qualified_path, single_schema_provider > schema_provider_map;
		typedef std::shared_ptr< schema_provider_map > schema_provider_map_handle;
	}
}



#endif


