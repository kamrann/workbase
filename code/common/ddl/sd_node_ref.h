// sd_node_ref.h

#ifndef __WB_DDL_SD_NODE_REF_H
#define __WB_DDL_SD_NODE_REF_H

#include "node_ref.h"
#include "sd_tree.h"

#include <functional>


namespace ddl {

	struct sd_node_ref
	{
		sd_tree::node_descriptor nd;

		sd_node_ref():
			nd(sd_tree::null_vertex())
		{}

		sd_node_ref(sd_tree::node_descriptor n):
			nd(n)
		{}

		operator bool() const
		{
			return nd != sd_tree::null_vertex();
		}
	};

	struct sd_node_ref_compare
	{
		inline bool operator() (sd_node_ref const& lhs, sd_node_ref const& rhs) const
		{
			return lhs.nd < rhs.nd;
		}
	};


	class navigator;

	typedef std::set< sd_node_ref, sd_node_ref_compare > sd_node_ref_list;

	typedef std::function< sd_node_ref_list(node_ref const&, navigator) > ref_resolver;

	sd_node_ref_list resolve_reference(node_ref const& ref, navigator nav);
	sd_node_ref resolve_reference_unique(node_ref const& ref, navigator nav);
	sd_node_ref unique_reference(sd_node_ref_list const& list);

}


#endif


