// defn_node_base_impl.h

#ifndef __WB_DDL_DEFN_NODE_BASE_IMPL_H
#define __WB_DDL_DEFN_NODE_BASE_IMPL_H

#include "../ddl_types.h"


namespace ddl {
	namespace detail {

		struct defn_node_base_impl
		{
			node_id id_;
			std::string name_;

			void set_id(node_id id)
			{
				id_ = id;
			}

//#ifdef _DEBUG
			void set_name(std::string name)
			{
				name_ = name;
			}
//#endif
		};

	}
}


#endif


