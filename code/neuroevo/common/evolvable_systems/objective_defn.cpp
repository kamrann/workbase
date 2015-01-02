// objective_defn.cpp

#include "objective_defn.h"
#include "single_objective.h"
#include "pareto_objective.h"

//#include "params/param_accessor.h"
//#include "params/dynamic_enum_schema.h"


namespace sys {
	namespace ev {

		objective_defn::objective_defn(state_val_access_fn_t& sv_acc_fn)//:
//			single_{ sv_acc_fn },
//			pareto_{ sv_acc_fn }
		{
			obj_type_defn_.register_option(
				"single_objective", std::make_shared< single_objective_defn >(sv_acc_fn));
			obj_type_defn_.register_option(
				"pareto_multi_objective", std::make_shared< pareto_objective_defn >(sv_acc_fn));
		}

		ddl::defn_node objective_defn::get_defn(ddl::specifier& spc)
		{
			return obj_type_defn_.get_defn(spc);

/*			ddl::defn_node obj_type = spc.enumeration("obj_type")
				(ddl::define_enum_fixed{}("single")("pareto"))
				;

			ddl::defn_node cond = spc.conditional("obj_contents_?")
				(ddl::spc_condition{ ddl::cnd::equal{ ddl::node_ref{ obj_type }, "single" }, single_.get_defn(spc) })
				(ddl::spc_condition{ ddl::cnd::equal{ ddl::node_ref{ obj_type }, "pareto" }, pareto_.get_defn(spc) })
				;

			return spc.composite("objective")(ddl::define_children{}
				("obj_type", obj_type)
				("obj_spec", cond)
				);
				*/
		}
		
		std::unique_ptr< objective > objective_defn::generate(ddl::navigator nav, std::function< double(state_value_id) > get_state_value_fn)
		{
			return obj_type_defn_.generate(nav, get_state_value_fn);

/*			auto type = nav["obj_type"].get().as_single_enum_str();
			if(type == "single")
			{
				nav = nav["obj_spec"][(size_t)0];
				return single_.generate(nav, get_state_value_fn);
			}
			else
			{
				nav = nav["obj_spec"][(size_t)0];
				return pareto_.generate(nav, get_state_value_fn);
			}
*/		}

	}
}




