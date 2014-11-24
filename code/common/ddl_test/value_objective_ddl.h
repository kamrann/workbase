// value_objective_ddl.h

#ifndef __VALUE_OBJECTIVE_DDL_H
#define __VALUE_OBJECTIVE_DDL_H

#include "ddl/ddl.h"


using namespace ddl;


struct value_objective_schema
{
	value_objective_schema(node_ref spec_list_ref):
		spec_list_ref_(spec_list_ref)
	{}

	defn_node get_defn(specifier& spc)
	{
		defn_node dir = spc.enumeration()
			(define_enum_fixed{}("maximise")("minimise"))
			(spc_range < size_t > { 1, 1 })
			(spc_default < enum_defn_node::value_t > { { "maximise" } })
			;

		// TODO: easier to catch this pointer, but can only do that if the object will be persistent
		auto spec_list_ref = spec_list_ref_;
		enum_defn_node::enum_values_fn_t state_val_fn = [spec_list_ref](navigator nav)
		{
			nav = nav_to_ref(spec_list_ref, nav);
			bool is_biped = false;
			auto num_specs = nav.list_num_items();
			for(size_t idx = 0; idx < num_specs; ++idx)
			{
				auto cnav = nav[idx];
				if(!cnav.get())
				{
					// Node may have just been added to list but not yet initialized
					continue;
				}
				auto spec_type_node = cnav[node_name{ "spec_type" }].get();
				// TODO: What this check should really be is: if(!unspecified)
				if(spec_type_node.as_enum().empty())
				{
					continue;
				}
				auto spec_type = spec_type_node.as_single_enum();
				if(spec_type == "biped")
				{
					is_biped = true;
					break;
				}
			}

			if(is_biped)
			{
				return enum_defn_node::enum_set_t{ "left_knee", "right_knee" };
			}
			else
			{
				return enum_defn_node::enum_set_t{ "whatevs" };
			}
		};

		defn_node val = spc.enumeration()
			(define_enum_func{ state_val_fn })
			;

		defn_node type = spc.enumeration()(define_enum_fixed{}
			("value")
			("average")
			("sum")
			("minimum")
			("maximum")
			);

		defn_node at_until_tp = spc.enumeration()(define_enum_fixed{}("end")("when_first")("when_last"));
		defn_node from_tp = spc.enumeration()(define_enum_fixed{}("start")("when_first")("when_last"));
		defn_node tp_range = spc.composite()(define_children{}
			("from", from_tp)
			("until", at_until_tp)
			);

		defn_node cond_nd = spc.conditional()
			(spc_condition{ cnd::equal{ node_ref{ type }, "value" }, at_until_tp })
			(spc_default_condition{ tp_range })
			;

		defn_node value_obj = spc.composite()(define_children{}
			("direction", dir)
			("value", val)
			("type", type)
			("timepoint", cond_nd)
			);

		return value_obj;
	}

	node_ref spec_list_ref_;
};


#endif


