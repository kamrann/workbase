// test_ddl.h

#ifndef __TEST_DDL_H
#define __TEST_DDL_H

#include "ddl/ddl.h"


using namespace ddl;


struct test_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node bool1 = spc.boolean("bool_1");

		defn_node int1 = spc.integer("int_1")
			(ddl::spc_default< integer_defn_node::value_t >{ 0 })
			;

		auto rf_bool1 = ddl::node_ref{ bool1 };
		auto cond = ddl::cnd::custom_condition{
			[rf_bool1](ref_resolver rr, navigator nav) -> ddl::cnd::ConditionResult
			{
				auto nd = rr(rf_bool1, nav);
				return nav.tree_->node_attribs(nd.begin()->nd).data.as_bool() ?
					ddl::cnd::ConditionResult::Pass :
					ddl::cnd::ConditionResult::Fail;
			}
		};
		cond.add_dependency(ddl::node_dependency(rf_bool1));
		defn_node cond1 = spc.conditional("cond_1")
			(ddl::spc_condition{ cond, int1 })
			;

		auto rf_int1 = ddl::node_ref{ int1 };
		auto enum1_values_fn = ddl::enum_defn_node::enum_values_fn_t{ [rf_bool1, rf_int1](ddl::navigator nav) -> ddl::enum_defn_node::enum_set_t
		{
			auto nav_bool = ddl::nav_to_ref(rf_bool1, nav);
			auto bool_val = nav_bool.get().as_bool();
			if(bool_val)
			{
				auto nav_int = ddl::nav_to_ref(rf_int1, nav);
				if(nav_int)
				{
					auto int_val = nav_int.get().as_int();
					return{ std::to_string(int_val) };
				}
				else
				{
					return{ "bool true but int not found" };
				}
			}
			else
			{
				return{ "bool not true" };
			}
		}
		};
		enum1_values_fn.add_dependency(ddl::node_dependency(rf_bool1));
		enum1_values_fn.add_dependency(ddl::node_dependency(rf_int1));

		defn_node enum1 = spc.enumeration("enum_1")
			(define_enum_func{ enum1_values_fn })
			;

		return spc.composite("test_root")(define_children{}
			("bool_1", bool1)
			("cond_1", cond1)
			("an_enum_1", enum1)
			);
	}
};


#endif


