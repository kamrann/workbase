// schema.h

#ifndef __WB_PARAM_SCHEMA_H
#define __WB_PARAM_SCHEMA_H

#include "param.h"

#include <functional>
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


		// Generates a default param from a schema
		param generate_default(schema_node sch);

		// Validate a param value against a schema
		bool validate_param(param const& p, schema_node sch);
	}

}



#endif


